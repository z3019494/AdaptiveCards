using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdaptiveCards
{
    static class ParseContext
    {
        public enum ContextType { Element, Action };

        public static ContextType Type { get; set; }

    }
}
