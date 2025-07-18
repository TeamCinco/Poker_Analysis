import numpy as np
import pandas as pd
from datetime import datetime, timedelta
from typing import List, Dict, Tuple
import json

class PokerAnalytics:
    def __init__(self, sessions: List[Dict]):
        self.sessions = sessions
        self.df = self._create_dataframe()
    
    def _create_dataframe(self) -> pd.DataFrame:
        """Convert sessions to DataFrame for analysis"""
        if not self.sessions:
            return pd.DataFrame()
        
        data = []
        for session in self.sessions:
            data.append({
                'date': pd.to_datetime(session['date']),
                'buy_in': session['buy_in'],
                'cash_out': session['cash_out'],
                'fees': session['fees'],
                'profit_loss': session['profit_loss'],
                'new_deposit': session.get('new_deposit', session['buy_in']),
                'roi': (session['profit_loss'] / session['buy_in']) * 100 if session['buy_in'] > 0 else 0
            })
        
        df = pd.DataFrame(data)
        df['cumulative_pl'] = df['profit_loss'].cumsum()
        df['session_number'] = range(1, len(df) + 1)
        return df
    
    def basic_stats(self) -> Dict:
        """Calculate basic statistical metrics"""
        if self.df.empty:
            return {}
        
        pl_series = self.df['profit_loss']
        return {
            'total_sessions': len(self.df),
            'win_rate': (pl_series > 0).mean() * 100,
            'mean_profit_loss': pl_series.mean(),
            'median_profit_loss': pl_series.median(),
            'std_dev': pl_series.std(),
            'variance': pl_series.var(),
            'skewness': pl_series.skew(),
            'kurtosis': pl_series.kurtosis(),
            'max_win': pl_series.max(),
            'max_loss': pl_series.min(),
            'profit_factor': abs(pl_series[pl_series > 0].sum() / pl_series[pl_series < 0].sum()) if pl_series[pl_series < 0].sum() != 0 else float('inf')
        }
    
    def risk_metrics(self) -> Dict:
        """Calculate risk-adjusted metrics"""
        if self.df.empty or len(self.df) < 2:
            return {}
        
        pl_series = self.df['profit_loss']
        cumulative_pl = self.df['cumulative_pl']
        
        # Calculate drawdown
        running_max = cumulative_pl.expanding().max()
        drawdown = cumulative_pl - running_max
        max_drawdown = drawdown.min()
        
        # Sharpe-like ratio (assuming risk-free rate = 0)
        sharpe_ratio = pl_series.mean() / pl_series.std() if pl_series.std() > 0 else 0
        
        # Value at Risk (95% confidence)
        var_95 = np.percentile(pl_series, 5)
        
        # Expected Shortfall (Conditional VaR)
        es_95 = pl_series[pl_series <= var_95].mean()
        
        return {
            'max_drawdown': max_drawdown,
            'current_drawdown': drawdown.iloc[-1],
            'sharpe_ratio': sharpe_ratio,
            'var_95': var_95,
            'expected_shortfall_95': es_95,
            'volatility': pl_series.std(),
            'downside_deviation': pl_series[pl_series < 0].std()
        }
    
    def streak_analysis(self) -> Dict:
        """Analyze winning and losing streaks"""
        if self.df.empty:
            return {}
        
        pl_series = self.df['profit_loss']
        
        # Calculate streaks
        wins = (pl_series > 0).astype(int)
        losses = (pl_series < 0).astype(int)
        
        def get_streaks(series):
            streaks = []
            current_streak = 0
            for val in series:
                if val == 1:
                    current_streak += 1
                else:
                    if current_streak > 0:
                        streaks.append(current_streak)
                    current_streak = 0
            if current_streak > 0:
                streaks.append(current_streak)
            return streaks
        
        win_streaks = get_streaks(wins)
        loss_streaks = get_streaks(losses)
        
        return {
            'max_win_streak': max(win_streaks) if win_streaks else 0,
            'max_loss_streak': max(loss_streaks) if loss_streaks else 0,
            'avg_win_streak': np.mean(win_streaks) if win_streaks else 0,
            'avg_loss_streak': np.mean(loss_streaks) if loss_streaks else 0,
            'current_streak': self._current_streak()
        }
    
    def _current_streak(self) -> str:
        """Calculate current winning/losing streak"""
        if self.df.empty:
            return "No sessions"
        
        recent_results = self.df['profit_loss'].iloc[::-1]  # Reverse to start from most recent
        streak = 0
        streak_type = "winning" if recent_results.iloc[0] > 0 else "losing"
        
        for result in recent_results:
            if (result > 0 and streak_type == "winning") or (result < 0 and streak_type == "losing"):
                streak += 1
            else:
                break
        
        return f"{streak} session {streak_type} streak"
    
    def performance_by_period(self) -> Dict:
        """Analyze performance by time periods"""
        if self.df.empty:
            return {}
        
        df = self.df.copy()
        df['hour'] = df['date'].dt.hour
        df['day_of_week'] = df['date'].dt.day_name()
        df['month'] = df['date'].dt.month_name()
        
        return {
            'by_hour': df.groupby('hour')['profit_loss'].agg(['mean', 'count']).to_dict(),
            'by_day_of_week': df.groupby('day_of_week')['profit_loss'].agg(['mean', 'count']).to_dict(),
            'by_month': df.groupby('month')['profit_loss'].agg(['mean', 'count']).to_dict()
        }
    
    def monte_carlo_simulation(self, num_simulations: int = 1000, sessions_ahead: int = 100) -> Dict:
        """Run Monte Carlo simulation for future performance"""
        if self.df.empty:
            return {}
        
        pl_series = self.df['profit_loss']
        mean_pl = pl_series.mean()
        std_pl = pl_series.std()
        
        # Run simulations
        final_pls = []
        for _ in range(num_simulations):
            simulated_sessions = np.random.normal(mean_pl, std_pl, sessions_ahead)
            final_pl = simulated_sessions.sum()
            final_pls.append(final_pl)
        
        final_pls = np.array(final_pls)
        
        return {
            'mean_expected_pl': final_pls.mean(),
            'median_expected_pl': np.median(final_pls),
            'std_expected_pl': final_pls.std(),
            'prob_profit': (final_pls > 0).mean() * 100,
            'percentile_5': np.percentile(final_pls, 5),
            'percentile_95': np.percentile(final_pls, 95),
            'sessions_simulated': sessions_ahead,
            'simulations_run': num_simulations
        }
    
    def kelly_criterion(self) -> float:
        """Calculate optimal bet size using Kelly Criterion"""
        if self.df.empty:
            return 0
        
        wins = self.df[self.df['profit_loss'] > 0]['profit_loss']
        losses = self.df[self.df['profit_loss'] < 0]['profit_loss']
        
        if len(wins) == 0 or len(losses) == 0:
            return 0
        
        win_rate = len(wins) / len(self.df)
        avg_win = wins.mean()
        avg_loss = abs(losses.mean())
        
        # Kelly = (bp - q) / b, where b = avg_win/avg_loss, p = win_rate, q = 1-p
        if avg_loss == 0:
            return 0
        
        b = avg_win / avg_loss
        p = win_rate
        q = 1 - p
        
        kelly_fraction = (b * p - q) / b
        return max(0, kelly_fraction)  # Don't allow negative Kelly