using System;

namespace AdaptiveCardStockBot
{
    public class StockQuote
    {
        public string companyName { get; set; }

        public string primaryExchange { get; set; }

        public string symbol { get; set; }

        public long latestUpdate { get; set; }

        public string latestUpdateFormatted
        {
            get
            {
                var ti = TimeZoneInfo.FindSystemTimeZoneById("Eastern Standard Time");
                var dt = DateTimeOffset.FromUnixTimeMilliseconds(latestUpdate);
                var converted = TimeZoneInfo.ConvertTime(dt, ti);
                return string.Format("{0:M}, {0:t} EST", converted);
            }
        }

        public decimal latestPrice { get; set; }

        public decimal change { get; set; }

        public string changeArrow => change >= 0 ? "▲" : "▼";

        public string changeFormatted => $"{changeArrow} {change.ToString("F2")} ({changePercent.ToString("P")})";

        public string changeColor => change >= 0 ? "good" : "attention";

        public decimal changePercent { get; set; }

        public decimal open { get; set; }

        public decimal high { get; set; }

        public decimal low { get; set; }
    }
}
