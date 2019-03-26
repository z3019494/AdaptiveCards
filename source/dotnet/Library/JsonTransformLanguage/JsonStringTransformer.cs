using Antlr4.Runtime;
using Antlr4.Runtime.Misc;
using JsonTransformLanguage.Grammars;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace JsonTransformLanguage
{
    internal static class JsonStringTransformer
    {
        private const string ID_DATA = "$data";
        private const string ID_PROPS = "$props";

        internal static JToken Transform(string input, JsonTransformerContext context)
        {
            return EvaluateBinding(input, context);
        }

        private const string REGEX_TEMPLATE_EXPRESSION = @"{([^}]+)}";

        public static JToken EvaluateBinding(string bindingExpression, JsonTransformerContext context)
        {
            // If whole expression is template expression, don't do any string concatenation
            var fullMatch = Regex.Match(bindingExpression, "^" + REGEX_TEMPLATE_EXPRESSION + "$");
            if (fullMatch != null && fullMatch.Success)
            {
                return EvaluateTemplateExpression(fullMatch.Groups[1].Value, context);
            }

            Regex regex = new Regex(REGEX_TEMPLATE_EXPRESSION);
            return regex.Replace(bindingExpression, (m) =>
            {
                return EvaluateTemplateExpressionMatch(m, context);
            });
        }

        private static string EvaluateTemplateExpressionMatch(Match match, JsonTransformerContext context)
        {
            try
            {
                return EvaluateTemplateExpression(match.Groups[1].Value, context).Value<string>();
            }
            catch (Exception ex)
            {
                return "";
            }
        }

        private static JToken EvaluateTemplateExpression(string templateExpression, JsonTransformerContext context)
        {
            if (context.ReservedProperties.Data is JObject dataObj)
            {
                foreach (var prop in dataObj.Properties())
                {
                    context.ScriptEngine.SetGlobalValue(prop.Name, prop.Value);
                }
            }
            return "Not implemented";
        }

        /// <summary>
        /// Gets one level property
        /// </summary>
        /// <param name="data"></param>
        /// <param name="property"></param>
        /// <returns></returns>
        public static JToken GetProperty(JToken data, string property)
        {
            if (data is JObject dataObj && dataObj.TryGetValue(property, out JToken value))
            {
                return value;
            }

            return null;
        }
    }
}
