using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Globalization;
using System.Windows.Data;

namespace AdaptiveCards.Rendering.Wpf
{
    public class InputValidationRule : ValidationRule
    {
        public InputValidationRule(AdaptiveInput input, TextBox textBox)
        {
            Input = input;
            TextBox = textBox;
        }

        AdaptiveInput Input { get; set; }
        TextBox TextBox { get; set; }
        bool HasLeftBox { get; set; }

        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            string type = Input.Type;

            if(TextBox.Text == "" && Input.IsRequired)
            {
                return new ValidationResult(false, null);
            }
            else
            {
                return new ValidationResult(true, null);
            }
        }
    }
}
