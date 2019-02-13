using System;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;
using AdaptiveCards;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json.Linq;

namespace StockQuoteBot.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class QuoteController : ControllerBase
    {
        private readonly IHostingEnvironment _env;

        public QuoteController(IHostingEnvironment env)
        {
            _env = env;
        }

        [HttpGet]
        public ActionResult<string> Get()
        {
            return Content("Please provide a symbol param. E.g. /quote/msft");
        }

        [HttpGet("{symbol}")]
        public async Task<ActionResult<string>> Get(string symbol)
        {
            var http = new HttpClient();
            var response = await http.GetAsync($"https://api.iextrading.com/1.0/stock/{symbol}/quote");
            if (response.IsSuccessStatusCode)
            {
                dynamic quote = JObject.Parse(await response.Content.ReadAsStringAsync());
                quote.dateFormatted = DateTimeOffset.FromUnixTimeMilliseconds(Convert.ToInt64(quote.latestUpdate)).ToString("s") + "Z";
                var template = await System.IO.File.ReadAllTextAsync(Path.Combine(_env.ContentRootPath, "Templates/StockQuoteTemplate.json"));
                var parseResult = AdaptiveCard.ResolveFromJson(template, quote.ToString());
                return parseResult.Card.ToJson();
            }

            return Content("Unable to load");
        }
    }
}
