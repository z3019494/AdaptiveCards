// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace AdaptiveCards.Rendering.Wpf
{
    public static class AdaptiveTextInputRenderer
    {
        public static FrameworkElement Render(AdaptiveTextInput input, AdaptiveRenderContext context)
        {
            var textBox = new TextBox() { Text = input.Value };
            if (input.IsMultiline == true)
            {
                textBox.AcceptsReturn = true;

                textBox.TextWrapping = TextWrapping.Wrap;
                textBox.HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled;
            }

            if (input.MaxLength > 0)
            {
                textBox.MaxLength = input.MaxLength;
            }

            textBox.SetPlaceholder(input.Placeholder);
            textBox.Style = context.GetStyle($"Adaptive.Input.Text.{input.Style}");
            textBox.SetContext(input);
            context.InputBindings.Add(input.Id, () => textBox.Text);

            ValidationRule validationRule = new InputValidationRule(input, textBox);
            validationRule.ValidationStep = ValidationStep.UpdatedValue;

            textBox.BindingGroup = new BindingGroup();
            textBox.BindingGroup.ValidationRules.Add(validationRule);
            textBox.BindingGroup.ValidatesOnNotifyDataError = true;
            textBox.BindingGroup.NotifyOnValidationError = true;

            textBox.TextChanged += TextBox_TextChanged;
            

            return textBox;
        }

        private static void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            (sender as TextBox).BindingGroup.CommitEdit();
        }
    }
}
