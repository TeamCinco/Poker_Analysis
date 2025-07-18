import numpy as np
import pandas as pd
from typing import Dict, List, Tuple
from scipy import stats
from datetime import datetime, timedelta

class AlphaDecayAnalyzer:
    def __init__(self, sessions: List[Dict], lookback_periods=[10, 20, 50]):
        self.sessions = sessions
        self.df = self._create_dataframe()
        self.lookback_periods = lookback_periods
    
    def _create_dataframe(self) -> pd.DataFrame:
        """Convert sessions to DataFrame"""
        if not self.sessions:
            return pd.DataFrame()
        
        data = []
        for session in self.sessions:
            data.append({
                'date': pd.to_datetime(session['date']),
                'profit_loss': session['profit_loss'],
                'buy_in': session['buy_in'],
                'roi': (session['profit_loss'] / session['buy_in']) * 100 if session['buy_in'] > 0 else 0
            })
        
        df = pd.DataFrame(data)
        df['cumulative_pl'] = df['profit_loss'].cumsum()
        df['session_number'] = range(1, len(df) + 1)
        return df
    
    def rolling_performance_analysis(self) -> Dict:
        """Analyze performance over rolling windows"""
        if len(self.df) < max(self.lookback_periods):
            return {"error": f"Need at least {max(self.lookback_periods)} sessions"}
        
        results = {}
        
        for period in self.lookback_periods:
            # Rolling metrics
            rolling_mean = self.df['profit_loss'].rolling(window=period).mean()
            rolling_std = self.df['profit_loss'].rolling(window=period).std()
            rolling_sharpe = rolling_mean / rolling_std
            rolling_winrate = self.df['profit_loss'].rolling(window=period).apply(lambda x: (x > 0).mean() * 100)
            
            # Current vs historical comparison
            current_mean = rolling_mean.iloc[-1]
            historical_mean = rolling_mean.iloc[:-period].mean()
            
            current_sharpe = rolling_sharpe.iloc[-1]
            historical_sharpe = rolling_sharpe.iloc[:-period].mean()
            
            current_winrate = rolling_winrate.iloc[-1]
            historical_winrate = rolling_winrate.iloc[:-period].mean()
            
            results[f'{period}_session'] = {
                'current_avg_pl': current_mean,
                'historical_avg_pl': historical_mean,
                'pl_deterioration': ((current_mean - historical_mean) / abs(historical_mean) * 100) if historical_mean != 0 else 0,
                'current_sharpe': current_sharpe,
                'historical_sharpe': historical_sharpe,
                'sharpe_deterioration': ((current_sharpe - historical_sharpe) / abs(historical_sharpe) * 100) if historical_sharpe != 0 else 0,
                'current_winrate': current_winrate,
                'historical_winrate': historical_winrate,
                'winrate_deterioration': current_winrate - historical_winrate,
                'rolling_series': {
                    'mean': rolling_mean.tolist(),
                    'sharpe': rolling_sharpe.tolist(),
                    'winrate': rolling_winrate.tolist()
                }
            }
        
        return results
    
    def trend_analysis(self) -> Dict:
        """Detect trends in performance using linear regression"""
        if len(self.df) < 10:
            return {"error": "Need at least 10 sessions for trend analysis"}
        
        results = {}
        
        # Analyze different metrics for trends
        metrics = {
            'profit_loss': self.df['profit_loss'].values,
            'cumulative_pl': self.df['cumulative_pl'].values,
            'roi': self.df['roi'].values
        }
        
        session_numbers = self.df['session_number'].values
        
        for metric_name, metric_values in metrics.items():
            # Linear regression
            slope, intercept, r_value, p_value, std_err = stats.linregress(session_numbers, metric_values)
            
            # Trend strength classification
            if p_value < 0.01:
                significance = "Highly Significant"
            elif p_value < 0.05:
                significance = "Significant"
            elif p_value < 0.10:
                significance = "Marginally Significant"
            else:
                significance = "Not Significant"
            
            trend_direction = "Improving" if slope > 0 else "Deteriorating"
            
            results[metric_name] = {
                'slope': slope,
                'r_squared': r_value**2,
                'p_value': p_value,
                'trend_direction': trend_direction,
                'significance': significance,
                'trend_strength': abs(r_value)
            }
        
        return results
    
    def regime_change_detection(self) -> Dict:
        """Detect structural breaks in performance"""
        if len(self.df) < 20:
            return {"error": "Need at least 20 sessions for regime detection"}
        
        pl_series = self.df['profit_loss'].values
        
        # Simple regime detection using rolling means
        short_window = 5
        long_window = 15
        
        if len(pl_series) < long_window:
            return {"error": f"Need at least {long_window} sessions"}
        
        short_ma = pd.Series(pl_series).rolling(window=short_window).mean()
        long_ma = pd.Series(pl_series).rolling(window=long_window).mean()
        
        # Detect crossovers
        crossovers = []
        regime_changes = []
        
        for i in range(long_window, len(short_ma)):
            if not pd.isna(short_ma.iloc[i]) and not pd.isna(long_ma.iloc[i]):
                if short_ma.iloc[i-1] <= long_ma.iloc[i-1] and short_ma.iloc[i] > long_ma.iloc[i]:
                    crossovers.append(('bullish', i))
                elif short_ma.iloc[i-1] >= long_ma.iloc[i-1] and short_ma.iloc[i] < long_ma.iloc[i]:
                    crossovers.append(('bearish', i))
        
        # Current regime
        current_regime = "Neutral"
        if len(short_ma) > 0 and len(long_ma) > 0:
            if short_ma.iloc[-1] > long_ma.iloc[-1]:
                current_regime = "Good Performance"
            else:
                current_regime = "Poor Performance"
        
        return {
            'current_regime': current_regime,
            'recent_crossovers': crossovers[-3:] if crossovers else [],
            'total_regime_changes': len(crossovers),
            'short_ma_current': short_ma.iloc[-1] if len(short_ma) > 0 else 0,
            'long_ma_current': long_ma.iloc[-1] if len(long_ma) > 0 else 0
        }
    
    def volatility_clustering(self) -> Dict:
        """Analyze volatility patterns"""
        if len(self.df) < 10:
            return {"error": "Need at least 10 sessions"}
        
        pl_series = self.df['profit_loss']
        
        # Calculate rolling volatility
        rolling_vol_5 = pl_series.rolling(window=5).std()
        rolling_vol_10 = pl_series.rolling(window=10).std()
        
        # Current vs historical volatility
        current_vol_5 = rolling_vol_5.iloc[-1] if len(rolling_vol_5) > 0 else 0
        historical_vol_5 = rolling_vol_5.iloc[:-5].mean() if len(rolling_vol_5) > 5 else 0
        
        vol_regime = "Normal"
        if current_vol_5 > historical_vol_5 * 1.5:
            vol_regime = "High Volatility"
        elif current_vol_5 < historical_vol_5 * 0.7:
            vol_regime = "Low Volatility"
        
        return {
            'current_volatility_5session': current_vol_5,
            'historical_volatility_5session': historical_vol_5,
            'volatility_ratio': current_vol_5 / historical_vol_5 if historical_vol_5 > 0 else 1,
            'volatility_regime': vol_regime,
            'volatility_trend': rolling_vol_5.iloc[-5:].tolist() if len(rolling_vol_5) >= 5 else []
        }
    
    def alpha_decay_score(self) -> Dict:
        """Calculate overall alpha decay score"""
        rolling_perf = self.rolling_performance_analysis()
        trends = self.trend_analysis()
        regime = self.regime_change_detection()
        volatility = self.volatility_clustering()
        
        if any("error" in result for result in [rolling_perf, trends, regime, volatility] if isinstance(result, dict)):
            return {"error": "Insufficient data for alpha decay analysis"}
        
        # Calculate composite score (0-100, where 100 = severe decay)
        decay_score = 0
        
        # Factor 1: Recent performance vs historical (40% weight)
        if '20_session' in rolling_perf:
            pl_deterioration = rolling_perf['20_session']['pl_deterioration']
            sharpe_deterioration = rolling_perf['20_session']['sharpe_deterioration']
            winrate_deterioration = rolling_perf['20_session']['winrate_deterioration']
            
            # Normalize deterioration scores
            pl_component = max(0, min(40, -pl_deterioration)) if pl_deterioration < 0 else 0
            sharpe_component = max(0, min(20, -sharpe_deterioration / 2)) if sharpe_deterioration < 0 else 0
            winrate_component = max(0, min(20, -winrate_deterioration)) if winrate_deterioration < 0 else 0
            
            decay_score += pl_component + sharpe_component + winrate_component
        
        # Factor 2: Trend analysis (30% weight)
        if 'profit_loss' in trends:
            if trends['profit_loss']['trend_direction'] == 'Deteriorating':
                trend_strength = trends['profit_loss']['trend_strength']
                significance_multiplier = {'Highly Significant': 1.0, 'Significant': 0.7, 'Marginally Significant': 0.4, 'Not Significant': 0.1}
                multiplier = significance_multiplier.get(trends['profit_loss']['significance'], 0.1)
                decay_score += trend_strength * 30 * multiplier
        
        # Factor 3: Regime change (20% weight)
        if regime['current_regime'] == 'Poor Performance':
            decay_score += 20
        
        # Factor 4: Volatility increase (10% weight)
        if volatility['volatility_regime'] == 'High Volatility':
            vol_ratio = volatility['volatility_ratio']
            decay_score += min(10, (vol_ratio - 1) * 10)
        
        # Classify decay level
        if decay_score >= 70:
            decay_level = "SEVERE ALPHA DECAY"
            recommendation = "STOP PLAYING - Your edge has significantly deteriorated"
        elif decay_score >= 50:
            decay_level = "MODERATE ALPHA DECAY"
            recommendation = "REDUCE STAKES - Review strategy and take a break"
        elif decay_score >= 30:
            decay_level = "MILD ALPHA DECAY"
            recommendation = "CAUTION - Monitor closely and consider strategy review"
        elif decay_score >= 15:
            decay_level = "SLIGHT DETERIORATION"
            recommendation = "WATCH - Some performance decline detected"
        else:
            decay_level = "STABLE/IMPROVING"
            recommendation = "CONTINUE - Performance remains strong"
        
        return {
            'alpha_decay_score': decay_score,
            'decay_level': decay_level,
            'recommendation': recommendation,
            'component_scores': {
                'performance_deterioration': min(80, decay_score * 0.4),
                'negative_trend': min(30, decay_score * 0.3),
                'regime_change': 20 if regime['current_regime'] == 'Poor Performance' else 0,
                'volatility_increase': min(10, decay_score * 0.1)
            }
        }