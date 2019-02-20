using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using AdaptiveCards;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Bot.Builder;
using Microsoft.Bot.Schema;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using System.Linq;

namespace AdaptiveCardStockBot
{
    public partial class AdaptiveCardStockBotBot : IBot
    {
        private readonly ILogger _logger;
        private readonly IHostingEnvironment _env;

        public AdaptiveCardStockBotBot(ConversationState conversationState, ILoggerFactory loggerFactory, IHostingEnvironment env)
        {
            _logger = loggerFactory.CreateLogger<AdaptiveCardStockBotBot>();
            _env = env;
        }

        public async Task OnTurnAsync(ITurnContext turnContext, CancellationToken cancellationToken)
        {
            if (turnContext.Activity.Type == ActivityTypes.ConversationUpdate)
            {
                await HandleNewConversationStarted(turnContext);
            }
            else if (turnContext.Activity.Type == ActivityTypes.Message)
            {
                await SendStockQuoteAsync(turnContext);
            }
            else
            {
                await turnContext.SendActivityAsync(turnContext.Activity.Type.ToString());
            }
        }


        private async Task HandleNewConversationStarted(ITurnContext turnContext)
        {
            foreach (var member in turnContext.Activity.MembersAdded ?? new List<ChannelAccount>())
            {
                if (member.Id != turnContext.Activity.Recipient.Id)
                {
                    await SendWelcomeCardAsync(turnContext, member);
                }
            }
        }

        private async Task SendWelcomeCardAsync(ITurnContext turnContext, ChannelAccount member)
        {
            var suggestedSymbols = new List<string> { "MSFT", "GOOG", "AAPL" };

            var http = new HttpClient();
            var response = await http.GetAsync("https://api.iextrading.com/1.0/stock/market/list/gainers");
            if(response.IsSuccessStatusCode)
            {
                var gainers = JsonConvert.DeserializeObject<List<StockQuote>>(await response.Content.ReadAsStringAsync());
                suggestedSymbols = gainers.Take(4).Select(m => m.symbol).ToList();
            }

            var data = new
            {
                name = member.Name,
                suggestedSymbols
            };
       
            await SendAdaptiveCardReplyAsync(turnContext, "WelcomeTemplate.json", data);
        }

        private async Task SendStockQuoteAsync(ITurnContext turnContext)
        {
            var symbol = turnContext.Activity.Text;

            if (symbol.Equals("hello", StringComparison.OrdinalIgnoreCase))
            {
                await SendWelcomeCardAsync(turnContext, turnContext.Activity.From);
            }
            else
            {
                var httpClient = new HttpClient();
                var response = await httpClient.GetAsync($"https://api.iextrading.com/1.0/stock/{symbol}/quote");

                if (response.IsSuccessStatusCode)
                {
                    var quote = JsonConvert.DeserializeObject<StockQuote>(await response.Content.ReadAsStringAsync());

                    //await SendAdaptiveCardReplyOldAndBusted(turnContext, quote);
                    await SendAdaptiveCardReplyAsync(turnContext, "StockQuoteTemplate.json", quote);
                }
                else
                {
                    await turnContext.SendActivityAsync($"Sorry, I wasn't able to get any data for: {symbol}");
                }
            }
        }

        private async Task SendAdaptiveCardReplyOldAndBusted(ITurnContext turnContext, StockQuote quote)
        {
            var reply = turnContext.Activity.CreateReply();

            var card = new AdaptiveCard("1.0")
            {
                Body = new List<AdaptiveElement>
                {
                    new AdaptiveContainer
                    {
                        Items =
                        {
                            new AdaptiveTextBlock
                            {
                                Text = $"{quote.companyName} ({quote.primaryExchange}: {quote.symbol})",
                                Size = AdaptiveTextSize.Medium,
                                IsSubtle = true
                            },
                            new AdaptiveTextBlock
                            {
                                Text = $"{quote.latestUpdateFormatted}",
                                IsSubtle = true
                            },
                        }
                    },
                    new AdaptiveContainer
                    {
                        Spacing = AdaptiveSpacing.None,
                        Items =
                        {
                            new AdaptiveColumnSet
                            {
                                Columns =
                                {
                                    new AdaptiveColumn
                                    {
                                        Width = "stretch",
                                        Items =
                                        {
                                            new AdaptiveTextBlock
                                            {
                                                Text = $"{quote.latestPrice}",
                                                Size = AdaptiveTextSize.ExtraLarge
                                            },
                                            new AdaptiveTextBlock
                                            {
                                                Text = $"{quote.changeFormatted}",
                                                Color = Enum.Parse<AdaptiveTextColor>(quote.changeColor, true),
                                                Spacing = AdaptiveSpacing.None
                                            }
                                        }
                                    },
                                    new AdaptiveColumn
                                    {
                                        Width = "auto",
                                        Items =
                                        {
                                            new AdaptiveFactSet
                                            {
                                                Facts =
                                                {
                                                    new AdaptiveFact("Open", $"{quote.open}"),
                                                    new AdaptiveFact("High", $"{quote.high}"),
                                                    new AdaptiveFact("Low", $"{quote.low}")
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            };

            reply.Attachments.Add(new Attachment
            {
                ContentType = "application/vnd.microsoft.card.adaptive",
                Content = card
            });

            await turnContext.SendActivityAsync(reply);
        }

        private async Task SendAdaptiveCardReplyAsync(ITurnContext turnContext, string templateName, object data = null)
        {
            var reply = turnContext.Activity.CreateReply();

            var template = await File.ReadAllTextAsync(Path.Combine(_env.ContentRootPath, "Templates", templateName));

            var card = AdaptiveCard.FromJson(template, data).Card;

            reply.Attachments.Add(new Attachment()
            {
                ContentType = "application/vnd.microsoft.card.adaptive",
                Content = card
            });

            await turnContext.SendActivityAsync(reply);
        }
    }
}
