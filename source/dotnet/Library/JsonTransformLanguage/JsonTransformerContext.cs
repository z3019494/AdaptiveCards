using Jurassic;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

namespace JsonTransformLanguage
{
    public class JsonTransformerContext
    {
        public JsonTransformerContext(JToken rootData, Dictionary<string, JToken> additionalReservedProperties)
        {
            ParentIsArray = false;

            ScriptEngine = new ScriptEngine();
            InitializeTransformer();
        }

        public bool ParentIsArray { get; set; }

        public JsonTransformerWarnings Warnings { get; private set; } = new JsonTransformerWarnings();

        public ScriptEngine ScriptEngine;

        public JsonTransformerContext(JsonTransformerContext existingContext)
        {
            foreach (var p in this.GetType().GetTypeInfo().DeclaredProperties)
            {
                p.SetValue(this, p.GetValue(existingContext));
            }
        }

        private void InitializeTransformer()
        {
            var assembly = Assembly.GetExecutingAssembly();
            var resourceName = assembly.GetManifestResourceNames().First(i => i.EndsWith("transformer.js"));

            string transformerJs;
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            using (StreamReader reader = new StreamReader(stream))
            {
                transformerJs = reader.ReadToEnd();
            }

            ScriptEngine.Execute(transformerJs);
        }
    }
}
