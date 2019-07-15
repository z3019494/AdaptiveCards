# InputValidation

## Proposal Link
https://github.com/microsoft/AdaptiveCards/issues/3081

## Dependencies
We may take a dependency on WinUI 3.0 for UWP input validation

## Rendering

### Validating Inputs

#### UWP
In UWP there isn't currently XAML support for input validation. A future feature for UWP input validation is spec'd [here](https://github.com/microsoft/microsoft-ui-xaml-specs/pull/26), has already been implemented, and will ship with WinUI 3.0 which will go into preview at Ignite in Novemeber 2019. We need to consider whether to take that dependency or roll our own input validation.

#### .NET
In .NET, we can take advantage of exisiting input validation functionality provided by XAML. Specifically, we will add a validation rule to [FrameworkElement.BindingGroup.ValidationRules](https://docs.microsoft.com/en-us/dotnet/api/system.windows.data.bindinggroup.validationrules) to validate the isRequired and, for text inputs, regex. We will then set an error template which adds an additional border around the input in the correct color from the host config.

### Show Cards
Currently, inputs from the entire card are collected and validated when a submit action is pressed, including all show cards. With the advent of input validation, we need to ensure we don't block submit on valiation of inputs that may not be visible due to un-expanded show cards. 

BECKYNOTE: What about toggle?

## Backwards Compatibility Concerns
Input valiation will be ignored when sent to older versions of the renderer. No breaking changes are introduced to the existing schema or APIs as part of this feature.

## Warning and Error States
 - If a cards author sets an error message without marking it as "isRequired", or without setting a regex, a warning should be returned
 - If a regex is provided that is not a regular expression an error should be returned (?)

 ## Open Issues
 TODO

 ## Testing

### Object Model
All Object model test suites should be updated to validate the new properties. Specifically:
- inputNecessityIndicators property on the top level card.
- IsRequired and ErrorMessage properties on inputs
- Regex on Input.Text
- IgnoreInputValidation on Action.Submit

 Test suites that should be updated:
- Shared model, via the EverythingBagel tests.
- UWP Object Model unit tests
- Android object model tests.

Additionally, error and warning cases should be tested. For shared model platforms this testing will take place in the shared model unit tests.

### Samples
The following samples will be added to validate rendering of input validation:
- Action.Submit.IgnoreInputValidation.json file which contains required inputs and a submit action which ignores validation
- AdaptiveCard.InputNecessityIndicators.json file which sets this property on the top level card
- Input.Text.IsRequired.json file which has required and optional inputs
- Input.Text.Regex.json file which has both required and optional text inputs with regexs