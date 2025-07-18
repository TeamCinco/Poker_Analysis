from alpha_decay import AlphaDecayAnalyzer

class AlphaDisplay:
    @staticmethod
    def show_alpha_decay_analysis(sessions):
        """Display comprehensive alpha decay analysis"""
        analyzer = AlphaDecayAnalyzer(sessions)
        
        print("\n" + "="*70)
        print("🚨 ALPHA DECAY DETECTION ANALYSIS")
        print("="*70)
        
        # Overall Alpha Decay Score
        decay_analysis = analyzer.alpha_decay_score()
        if "error" not in decay_analysis:
            score = decay_analysis['alpha_decay_score']
            level = decay_analysis['decay_level']
            recommendation = decay_analysis['recommendation']
            
            print(f"\n🎯 ALPHA DECAY SCORE: {score:.1f}/100")
            print(f"📊 DECAY LEVEL: {level}")
            print(f"💡 RECOMMENDATION: {recommendation}")
            
            if score >= 30:
                print("\n⚠️  WARNING SIGNALS DETECTED!")
        
        # Rolling Performance Analysis
        rolling_perf = analyzer.rolling_performance_analysis()
        if "error" not in rolling_perf and '20_session' in rolling_perf:
            print("\n📈 ROLLING PERFORMANCE (Last 20 Sessions vs Historical):")
            data = rolling_perf['20_session']
            
            print(f"Average P&L: ${data['current_avg_pl']:.2f} vs ${data['historical_avg_pl']:.2f} "
                  f"({data['pl_deterioration']:+.1f}%)")
            print(f"Sharpe Ratio: {data['current_sharpe']:.2f} vs {data['historical_sharpe']:.2f} "
                  f"({data['sharpe_deterioration']:+.1f}%)")
            print(f"Win Rate: {data['current_winrate']:.1f}% vs {data['historical_winrate']:.1f}% "
                  f"({data['winrate_deterioration']:+.1f}%)")
        
        # Trend Analysis
        trends = analyzer.trend_analysis()
        if "error" not in trends:
            print("\n📉 TREND ANALYSIS:")
            for metric, data in trends.items():
                trend_emoji = "📈" if data['trend_direction'] == 'Improving' else "📉"
                print(f"{trend_emoji} {metric.replace('_', ' ').title()}: {data['trend_direction']} "
                      f"(R² = {data['r_squared']:.3f}, {data['significance']})")
        
        # Regime Detection
        regime = analyzer.regime_change_detection()
        if "error" not in regime:
            print(f"\n🔄 CURRENT REGIME: {regime['current_regime']}")
            if regime['recent_crossovers']:
                print("Recent regime changes:")
                for regime_type, session in regime['recent_crossovers']:
                    print(f"  • Session {session}: {regime_type.title()} crossover")
        
        # Volatility Analysis
        vol_analysis = analyzer.volatility_clustering()
        if "error" not in vol_analysis:
            print(f"\n📊 VOLATILITY REGIME: {vol_analysis['volatility_regime']}")
            print(f"Current vs Historical Volatility: {vol_analysis['volatility_ratio']:.2f}x")
        
        print("="*70)
    
    @staticmethod
    def show_performance_alerts(sessions):
        """Show immediate performance alerts"""
        if len(sessions) < 5:
            print("Need at least 5 sessions for alerts")
            return
        
        analyzer = AlphaDecayAnalyzer(sessions)
        recent_sessions = sessions[-5:]
        recent_pl = [s['profit_loss'] for s in recent_sessions]
        
        print("\n🚨 PERFORMANCE ALERTS:")
        print("-" * 40)
        
        # Recent losing streak
        current_losses = 0
        for pl in reversed(recent_pl):
            if pl < 0:
                current_losses += 1
            else:
                break
        
        if current_losses >= 3:
            print(f"⚠️  {current_losses} consecutive losing sessions!")
        
        # Large recent loss
        recent_loss = min(recent_pl)
        if recent_loss < -50:  # Customize threshold
            print(f"💸 Large recent loss detected: ${recent_loss:.2f}")
        
        # Volatility spike
        vol_analysis = analyzer.volatility_clustering()
        if "error" not in vol_analysis and vol_analysis['volatility_regime'] == 'High Volatility':
            print(f"📈 Volatility spike: {vol_analysis['volatility_ratio']:.1f}x normal levels")
        
        # Recent performance vs average
        recent_avg = np.mean(recent_pl)
        overall_avg = np.mean([s['profit_loss'] for s in sessions])
        
        if recent_avg < overall_avg * 0.5:  # Recent performance significantly worse
            print(f"📉 Recent performance declining: ${recent_avg:.2f} vs ${overall_avg:.2f} average")
        
        if current_losses == 0 and recent_loss >= 0 and recent_avg > 0:
            print("✅ No immediate alerts - performance stable")