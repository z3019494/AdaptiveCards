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
            ReservedProperties = new JsonTransformerReservedProperties()
            {
                RootData = rootData,
                Data = rootData,
                Index = null,
                Props = null,
                AdditionalReservedProperties = additionalReservedProperties ?? new Dictionary<string, JToken>()
            };
            ParentIsArray = false;
            Types = new JsonTransformerTypes();

            ScriptEngine = new ScriptEngine();
            InitializeTransformer();
            InitializeData(rootData);
        }

        public JsonTransformerReservedProperties ReservedProperties { get; set; }

        public bool ParentIsArray { get; set; }

        public JsonTransformerTypes Types { get; set; }

        public JsonTransformerWarnings Warnings { get; private set; } = new JsonTransformerWarnings();

        public ScriptEngine ScriptEngine;

        public JsonTransformerContext(JsonTransformerContext existingContext)
        {
            foreach (var p in this.GetType().GetTypeInfo().DeclaredProperties)
            {
                p.SetValue(this, p.GetValue(existingContext));
            }

            if (Types != null)
            {
                Types = Types.Clone();
            }

            if (ReservedProperties != null)
            {
                ReservedProperties = new JsonTransformerReservedProperties(ReservedProperties);
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

        private void InitializeData(JToken rootData)
        {
            string jsonData = rootData.ToString();
            ScriptEngine.SetGlobalValue("rootDataJson", jsonData);
            ScriptEngine.Execute("setRootData(rootDataJson);");
        }
    }
}
