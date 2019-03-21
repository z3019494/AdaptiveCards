using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdaptiveCards
{
    public class AdaptiveUnknownElement: AdaptiveElement
    {
        public override string Type { get; set; }
    }

    public class AdaptiveUnknownAction: AdaptiveAction
    {
        public override string Type { get; set; }
    }
}
