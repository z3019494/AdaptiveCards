using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;

namespace JsonTransformLanguage
{
    public static class JsonTransformer
    {
        public static JToken Transform(JToken input, JToken data, Dictionary<string, JToken> additionalReservedProperties)
        {
            var context = new JsonTransformerContext(data, additionalReservedProperties);
            context.ScriptEngine.SetGlobalValue("rootDataJson", ToJson(data));
            context.ScriptEngine.SetGlobalValue("templateJson", ToJson(input));
            string answer = context.ScriptEngine.Evaluate<string>("transform(templateJson, rootDataJson)");
            var tokenAnswer = JToken.Parse(answer);

            // Remove all $data and $when
            Sanitize(tokenAnswer);
            return tokenAnswer;

            //return Transform(input, new JsonTransformerContext(data, additionalReservedProperties));
        }

        private static string ToJson(JToken token)
        {
            string answer = token.ToString();
            if (string.IsNullOrWhiteSpace(answer))
            {
                return "{}";
            }
            return answer;
        }

        private static void Sanitize(JToken token)
        {
            if (token is JObject jObj)
            {
                foreach (var prop in jObj.Properties().ToArray())
                {
                    if (prop.Name == "$data" || prop.Name == "$when")
                    {
                        jObj.Remove(prop.Name);
                    }
                    else
                    {
                        Sanitize(prop.Value);
                    }
                }
            }
            else if (token is JArray jArray)
            {
                foreach (var t in jArray.Children())
                {
                    Sanitize(t);
                }
            }
        }
    }
}
