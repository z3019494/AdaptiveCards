using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;

namespace AdaptiveCards.Rendering.Html.Test
{
    [TestClass]
    public class HtmlRendererTests
    {
        [TestMethod]
        public void TextBlockRender_ParagraphElementStylesAdded()
        {
            var renderContext = new AdaptiveRenderContext(
                new AdaptiveHostConfig(),
                new AdaptiveElementRenderers<HtmlTag, AdaptiveRenderContext>());

            var textBlock = new AdaptiveTextBlock
            {
                Text = "first line\n\nsecond line",
            };

            var generatedHtml = TestHtmlRenderer.CallTextBlockRender(textBlock, renderContext).ToString();

            // From String

            // Generated HTML should have two <p> tags, with appropriate styles set.
            Assert.AreEqual(
                "<div class=\'ac-textblock\' style=\'box-sizing: border-box;text-align: left;color: rgba(0, 0, 0, 1.00);line-height: 18.62px;font-size: 14px;font-weight: 400;white-space: nowrap;\'><p style=\'margin-top: 0px;margin-bottom: 0px;width: 100%;text-overflow: ellipsis;overflow: hidden;\'>first line</p><p style=\'margin-top: 0px;margin-bottom: 0px;width: 100%;text-overflow: ellipsis;overflow: hidden;\'>second line</p></div>",
                generatedHtml);
        }

        [TestMethod]
        public void TestUrlEncodingProperly()
        {
            var cardJson = @"{  
    ""type"":""AdaptiveCard"",
    ""version"":""1.0"",
    ""body"":[
         {  
            ""type"":""Image"",
            ""url"":""https://www.bing.com/th?u=https%3a%2f%2fsafetymoments-a1ea.azurewebsites.net%2fimages%2fOsha1.jpg&ehk=A8%2bdSDm336vG%2b3Vw4xoRUQ&p=0&pid=CortSkills&w=360""
        }
    ],
    ""actions"":[
        {  
            ""type"":""Action.OpenUrl"",
            ""url"":""https://www.osha.gov/SLTC/fallprotection/index.html"",
            ""title"":""More at www.osha.gov""
        },
        {  
            ""type"":""Action.OpenUrl"",
            ""url"":""http://www.octavianit.com/p/safetymoments.html"",
            ""title"":""About Safety Moments""
        }
    ]
}";

            var imageJson = @" {  
            ""type"":""Image"",
            ""url"":""https://www.bing.com/th?u=https%3a%2f%2fsafetymoments-a1ea.azurewebsites.net%2fimages%2fOsha1.jpg&ehk=A8%2bdSDm336vG%2b3Vw4xoRUQ&p=0&pid=CortSkills&w=360""
        }";



            var renderContext = new AdaptiveRenderContext(
              new AdaptiveHostConfig(),
              new AdaptiveElementRenderers<HtmlTag, AdaptiveRenderContext>());

            var image = JsonConvert.DeserializeObject<AdaptiveImage>(imageJson);

            //var image = new AdaptiveImage
            //{
            //    Url = new System.Uri("https://www.bing.com/th?u=https%3a%2f%2fsafetymoments-a1ea.azurewebsites.net%2fimages%2fOsha1.jpg&ehk=A8%2bdSDm336vG%2b3Vw4xoRUQ&p=0&pid=CortSkills&w=360")
            //};


            var imageHtml = TestHtmlRenderer.CallImageRender(image, renderContext).ToString();
            StringAssert.Contains(imageHtml, "https://www.bing.com/th?u=https%3a%2f%2fsafetymoments-a1ea.azurewebsites.net%2fimages%2fOsha1.jpg&ehk=A8%2bdSDm336vG%2b3Vw4xoRUQ&p=0&pid=CortSkills&w=360");

        }


        private class TestHtmlRenderer : AdaptiveCardRenderer
        {
            public TestHtmlRenderer(AdaptiveHostConfig config)
                : base(config)
            {
            }

            public static HtmlTag CallTextBlockRender(AdaptiveTextBlock element, AdaptiveRenderContext context)
            {
                return TextBlockRender(element, context);
            }

            public static HtmlTag CallImageRender(AdaptiveImage element, AdaptiveRenderContext context)
            {
                return ImageRender(element, context);
            }

            public static HtmlTag CallChoiceSetInputRender(AdaptiveChoiceSetInput element, AdaptiveRenderContext context)
            {
                return ChoiceSetRender(element, context);
            }
        }

        [TestMethod]
        public void ChoiceSetInput()
        {
            var renderContext = new AdaptiveRenderContext(
                new AdaptiveHostConfig(),
                new AdaptiveElementRenderers<HtmlTag, AdaptiveRenderContext>());

            var dropdownList = new AdaptiveChoiceSetInput()
            {
                Id = "1",
                Value = "1,3",
                Style = AdaptiveChoiceInputStyle.Compact,
                Choices =
                {
                    new AdaptiveChoice()
                    {
                        Title = "Value 1",
                        Value = "1"
                    },
                    new AdaptiveChoice()
                    {
                        Title = "Value 2",
                        Value = "2"
                    },
                    new AdaptiveChoice()
                    {
                        Title = "Value 3",
                        Value = "3"
                    }
                }
            };

            var dropdownGeneratedHtml = TestHtmlRenderer.CallChoiceSetInputRender(dropdownList, renderContext).ToString();

            // Generated HTML should have an additional disabled and hidden option which is selected.
            Assert.AreEqual(
                "<select class='ac-input ac-multichoiceInput' name='1' style='width: 100%;'><option disabled='' hidden='' selected=''/><option value='1'>Value 1</option><option value='2'>Value 2</option><option value='3'>Value 3</option></select>",
                dropdownGeneratedHtml);
        }
    }
}
