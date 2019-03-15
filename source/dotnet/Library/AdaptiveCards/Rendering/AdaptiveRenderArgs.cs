using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// This class contais all proterties that are used for rendering and need to be passed down between parent and child elements 
namespace AdaptiveCards.Rendering
{
    public enum DirectionChildrenCanBleedTowardsEnum { None, Left, Right, Both };

    public class AdaptiveRenderArgs
    {
        public AdaptiveRenderArgs() { }

        public AdaptiveRenderArgs(AdaptiveRenderArgs previousRenderArgs)
        {
            ParentStyle = previousRenderArgs.ParentStyle;
            ForegroundColors = previousRenderArgs.ForegroundColors;
            HasParentWithPadding = previousRenderArgs.HasParentWithPadding;
            DirectionChildrenCanBleedToward = previousRenderArgs.DirectionChildrenCanBleedToward;
        }

        public AdaptiveContainerStyle ParentStyle { get; set; } = AdaptiveContainerStyle.Default;

        public ForegroundColorsConfig ForegroundColors { get; set; }
        
        public DirectionChildrenCanBleedTowardsEnum DirectionChildrenCanBleedToward { get; set; }

        public bool HasParentWithPadding { get; set; } = true;

    }
}
