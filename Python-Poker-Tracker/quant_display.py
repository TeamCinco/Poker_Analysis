from analytics import PokerAnalytics

class QuantDisplay:
    @staticmethod
    def show_quant_analysis(sessions):
        """Display comprehensive quantitative analysis"""
        analytics = PokerAnalytics(sessions)
        
        print("\n" + "="*60)
        print("QUANTITATIVE POKER ANALYSIS")
        print("="*60)
        
        # Basic Stats
        basic = analytics.basic_stats()
        if basic:
            print("\n📊 BASIC STATISTICS:")
            print(f"Total Sessions: {basic['total_sessions']}")
            print(f"Win Rate: {basic['win_rate']:.1f}%")
            print(f"Mean P&L: ${basic['mean_profit_loss']:.2f}")
            print(f"Median P&L: ${basic['median_profit_loss']:.2f}")
            print(f"Std Deviation: ${basic['std_dev']:.2f}")
            print(f"Skewness: {basic['skewness']:.2f}")
            print(f"Kurtosis: {basic['kurtosis']:.2f}")
            print(f"Max Win: ${basic['max_win']:.2f}")
            print(f"Max Loss: ${basic['max_loss']:.2f}")
            print(f"Profit Factor: {basic['profit_factor']:.2f}")
        
        # Risk Metrics
        risk = analytics.risk_metrics()
        if risk:
            print("\n📉 RISK METRICS:")
            print(f"Max Drawdown: ${risk['max_drawdown']:.2f}")
            print(f"Current Drawdown: ${risk['current_drawdown']:.2f}")
            print(f"Sharpe Ratio: {risk['sharpe_ratio']:.2f}")
            print(f"Volatility: ${risk['volatility']:.2f}")
            print(f"VaR (95%): ${risk['var_95']:.2f}")
            print(f"Expected Shortfall: ${risk['expected_shortfall_95']:.2f}")
        
        # Streak Analysis
        streaks = analytics.streak_analysis()
        if streaks:
            print("\n🔥 STREAK ANALYSIS:")
            print(f"Max Win Streak: {streaks['max_win_streak']} sessions")
            print(f"Max Loss Streak: {streaks['max_loss_streak']} sessions")
            print(f"Current Streak: {streaks['current_streak']}")
        
        # Kelly Criterion
        kelly = analytics.kelly_criterion()
        if kelly > 0:
            print(f"\n💰 OPTIMAL BET SIZE (Kelly): {kelly:.1%} of bankroll")
        
        # Monte Carlo
        print("\n🎲 MONTE CARLO SIMULATION (next 100 sessions):")
        mc = analytics.monte_carlo_simulation()
        if mc:
            print(f"Expected P&L: ${mc['mean_expected_pl']:.2f}")
            print(f"Probability of Profit: {mc['prob_profit']:.1f}%")
            print(f"5th Percentile: ${mc['percentile_5']:.2f}")
            print(f"95th Percentile: ${mc['percentile_95']:.2f}")
        
        print("="*60)
    
    @staticmethod
    def show_performance_breakdown(sessions):
        """Show performance by different time periods"""
        analytics = PokerAnalytics(sessions)
        perf = analytics.performance_by_period()
        
        if not perf:
            print("No data available for performance breakdown")
            return
        
        print("\n" + "="*50)
        print("PERFORMANCE BY TIME PERIOD")
        print("="*50)
        
        if 'by_hour' in perf:
            print("\n⏰ BY HOUR OF DAY:")
            for hour, stats in perf['by_hour']['mean'].items():
                count = perf['by_hour']['count'][hour]
                print(f"{hour:2d}:00 - Avg: ${stats:6.2f} ({count} sessions)")
        
        if 'by_day_of_week' in perf:
            print("\n📅 BY DAY OF WEEK:")
            for day, stats in perf['by_day_of_week']['mean'].items():
                count = perf['by_day_of_week']['count'][day]
                print(f"{day:9s} - Avg: ${stats:6.2f} ({count} sessions)")
        
        print("="*50)