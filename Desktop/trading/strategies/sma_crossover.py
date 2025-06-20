import backtrader as bt
import yfinance as yf
import os
import pandas as pd

class SmaCross(bt.Strategy):
    params = dict(period=20)

    def __init__(self):
        sma = bt.ind.SMA(period=self.p.period)
        self.crossover = bt.ind.CrossOver(self.data.close, sma)

    def next(self):
        if not self.position and self.crossover > 0:
            self.buy(size=100)
        elif self.position and self.crossover < 0:
            self.close()

if __name__ == '__main__':
    cerebro = bt.Cerebro()
    cerebro.addstrategy(SmaCross, period=20)

    # 1) Download and prepare data using pandas
    datafile = os.path.expanduser('data/AAPL_clean.csv')
    if not os.path.isfile(datafile):
        df = yf.download("AAPL", start="2020-01-01", end="2023-01-01")
        # Create clean dataframe with just the columns we need
        clean_df = df[['Open', 'High', 'Low', 'Close', 'Volume']].reset_index()
        clean_df.to_csv(datafile, index=False, header=True)
    else:
        # Read and clean data using pandas
        print("Loading and cleaning data with pandas...")
        df = pd.read_csv(datafile)
        # Drop any rows with missing values
        df = df.dropna()
        # Ensure date column is properly formatted
        df['Date'] = pd.to_datetime(df['Date'])
        # Save cleaned data back
        df.to_csv(datafile, index=False)
        print(f"Cleaned data file - kept {len(df)} valid rows")

    # 2) Feed to Backtrader using PandasData
    try:
        # Create pandas dataframe from cleaned CSV
        df = pd.read_csv(datafile, parse_dates=['Date'], index_col='Date')
        # Verify column names
        print("Data columns:", df.columns)
        
        # Use 0-based column indices for PandasData
        data = bt.feeds.PandasData(
            dataname=df,
            fromdate=pd.to_datetime('2020-01-01'),
            todate=pd.to_datetime('2023-01-01'),
            open=0,    # First column after index
            high=1,
            low=2,
            close=3,
            volume=4)
    except Exception as e:
        print(f"Error loading data: {e}")
        print("Please check the data file format and try again")
        exit(1)
    cerebro.adddata(data)

    # 3) Add analyzers
    cerebro.addanalyzer(bt.analyzers.SharpeRatio, _name='sharpe')
    cerebro.addanalyzer(bt.analyzers.DrawDown, _name='drawdown')
    cerebro.addanalyzer(bt.analyzers.Returns, _name='returns')
    cerebro.addanalyzer(bt.analyzers.TradeAnalyzer, _name='trades')
    cerebro.addanalyzer(bt.analyzers.PyFolio, _name='pyfolio')

    # 4) Run and collect results
    print("Starting backtest...")
    results = cerebro.run()
    strat = results[0]

    # Print detailed performance metrics
    print("\n=== Backtest Results ===")
    
    # Sharpe Ratio
    sharpe = strat.analyzers.sharpe.get_analysis()
    print('Sharpe Ratio:', sharpe.get('sharperatio', 'N/A'))
    
    # DrawDown
    drawdown = strat.analyzers.drawdown.get_analysis()
    print('Max Drawdown:', drawdown.get('max', {}).get('drawdown', 'N/A'))
    
    # Returns - handle different versions
    rets = strat.analyzers.returns.get_analysis()
    print('Annual Return:', rets.get('rnorm100', rets.get('rnorm', 'N/A')))
    print('Total Return:', rets.get('rtot100', rets.get('rtot', 'N/A')))
    
    # Trade statistics with robust error handling
    trades = strat.analyzers.trades.get_analysis()
    print('\n=== Trade Statistics ===')
    
    total_trades = trades.get('total', {}).get('closed', 0)
    print('Total Trades:', total_trades)
    
    if total_trades > 0:
        # Win Rate
        won = trades.get('won', {}).get('total', 0)
        print('Win Rate:', f"{won/total_trades:.2%}")
        
        # Profit Factor
        pnl = trades.get('pnl', {}).get('net', {})
        profit = pnl.get('profit', 0)
        loss = pnl.get('loss', 0)
        
        if loss != 0:
            print('Profit Factor:', f"{profit/loss:.2f}")
        elif profit > 0:
            print('Profit Factor: Infinite (no losses)')
        else:
            print('Profit Factor: N/A')
    else:
        print('No trades were completed')

    # 5) Plot equity curve with more details
    print("\nGenerating performance charts...")
    cerebro.plot(style='candlestick', volume=False)
