import * as Core from "../card-elements";
import { Label } from "./label";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";
import { AdaptiveCard } from "../elements/adaptive-card";

export class Choice {
	title: string;
	value: string;

	constructor(title: string = undefined, value: string = undefined) {
		this.title = title;
		this.value = value;
	}

	toJSON() {
		return { title: this.title, value: this.value };
	}
}

export class ChoiceSetInput extends Core.Input {
	private static uniqueCategoryCounter = 0;

	private static getUniqueCategoryName(): string {
		let uniqueCwtegoryName = "__ac-category" + ChoiceSetInput.uniqueCategoryCounter;

		ChoiceSetInput.uniqueCategoryCounter++;

		return uniqueCwtegoryName;
	}

	private _selectElement: HTMLSelectElement;
	private _toggleInputs: Array<HTMLInputElement>;

	protected internalRender(): HTMLElement {
		if (!this.isMultiSelect) {
			if (this.isCompact) {
				// Render as a combo box
				this._selectElement = document.createElement("select");
				this._selectElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-multichoiceInput");
				this._selectElement.style.width = "100%";

				let option = document.createElement("option");
				option.selected = true;
				option.disabled = true;
				option.hidden = true;
				option.value = "";

				if (this.placeholder) {
					option.text = this.placeholder;
				}

				Utils.appendChild(this._selectElement, option);

				for (var i = 0; i < this.choices.length; i++) {
					let option = document.createElement("option");
					option.value = this.choices[i].value;
					option.text = this.choices[i].title;
					option.setAttribute("aria-label", this.choices[i].title);

					if (this.choices[i].value == this.defaultValue) {
						option.selected = true;
					}

					Utils.appendChild(this._selectElement, option);
				}

				this._selectElement.onchange = () => { this.valueChanged(); }

				return this._selectElement;
			}
			else {
				// Render as a series of radio buttons
				let uniqueCategoryName = ChoiceSetInput.getUniqueCategoryName();

				let element = document.createElement("div");
				element.className = this.hostConfig.makeCssClassName("ac-input");
				element.style.width = "100%";

				this._toggleInputs = [];

				for (let i = 0; i < this.choices.length; i++) {
					let radioInput = document.createElement("input");
					radioInput.id = Utils.generateUniqueId();
					radioInput.type = "radio";
					radioInput.style.margin = "0";
					radioInput.style.display = "inline-block";
					radioInput.style.verticalAlign = "middle";
					radioInput.name = Utils.isNullOrEmpty(this.id) ? uniqueCategoryName : this.id;
					radioInput.value = this.choices[i].value;
					radioInput.style.flex = "0 0 auto";
					radioInput.setAttribute("aria-label", this.choices[i].title);

					if (this.choices[i].value == this.defaultValue) {
						radioInput.checked = true;
					}

					radioInput.onchange = () => { this.valueChanged(); }

					this._toggleInputs.push(radioInput);

					let label = new Label();
					label.setParent(this);
					label.forElementId = radioInput.id;
					label.hostConfig = this.hostConfig;
					label.text = Utils.isNullOrEmpty(this.choices[i].title) ? "Choice " + i : this.choices[i].title;
					label.useMarkdown = AdaptiveCard.useMarkdownInRadioButtonAndCheckbox;

					let labelElement = label.render();
					labelElement.style.display = "inline-block";
					labelElement.style.flex = "1 1 auto";
					labelElement.style.marginLeft = "6px";
					labelElement.style.verticalAlign = "middle";

					let spacerElement = document.createElement("div");
					spacerElement.style.width = "6px";

					let compoundInput = document.createElement("div");
					compoundInput.style.display = "flex";

					Utils.appendChild(compoundInput, radioInput);
					Utils.appendChild(compoundInput, spacerElement);
					Utils.appendChild(compoundInput, labelElement);

					Utils.appendChild(element, compoundInput);
				}

				return element;
			}
		}
		else {
			// Render as a list of toggle inputs
			let defaultValues = this.defaultValue ? this.defaultValue.split(this.hostConfig.choiceSetInputValueSeparator) : null;

			let element = document.createElement("div");
			element.className = this.hostConfig.makeCssClassName("ac-input");
			element.style.width = "100%";

			this._toggleInputs = [];

			for (let i = 0; i < this.choices.length; i++) {
				let checkboxInput = document.createElement("input");
				checkboxInput.id = Utils.generateUniqueId();
				checkboxInput.type = "checkbox";
				checkboxInput.style.margin = "0";
				checkboxInput.style.display = "inline-block";
				checkboxInput.style.verticalAlign = "middle";
				checkboxInput.value = this.choices[i].value;
				checkboxInput.style.flex = "0 0 auto";
				checkboxInput.setAttribute("aria-label", this.choices[i].title);

				if (defaultValues) {
					if (defaultValues.indexOf(this.choices[i].value) >= 0) {
						checkboxInput.checked = true;
					}
				}

				checkboxInput.onchange = () => { this.valueChanged(); }

				this._toggleInputs.push(checkboxInput);

				let label = new Label();
				label.setParent(this);
				label.forElementId = checkboxInput.id;
				label.hostConfig = this.hostConfig;
				label.text = Utils.isNullOrEmpty(this.choices[i].title) ? "Choice " + i : this.choices[i].title;
				label.useMarkdown = AdaptiveCard.useMarkdownInRadioButtonAndCheckbox;

				let labelElement = label.render();
				labelElement.style.display = "inline-block";
				labelElement.style.flex = "1 1 auto";
				// labelElement.style.marginLeft = "6px";
				labelElement.style.verticalAlign = "middle";

				let spacerElement = document.createElement("div");
				spacerElement.style.width = "6px";

				let compoundInput = document.createElement("div");
				compoundInput.style.display = "flex";
				compoundInput.style.alignItems = "center";

				Utils.appendChild(compoundInput, checkboxInput);
				Utils.appendChild(compoundInput, spacerElement);
				Utils.appendChild(compoundInput, labelElement);

				Utils.appendChild(element, compoundInput);
			}

			return element;
		}
	}

	choices: Array<Choice> = [];
	isCompact: boolean;
	isMultiSelect: boolean;
	placeholder: string;

	getJsonTypeName(): string {
		return "Input.ChoiceSet";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "placeholder", this.placeholder);

		if (this.choices.length > 0) {
			var choices = [];

			for (let choice of this.choices) {
				choices.push(choice.toJSON());
			}

			Utils.setProperty(result, "choices", choices);
		}

		if (!this.isCompact) {
			Utils.setProperty(result, "style", "expanded", false);
		}

		Utils.setProperty(result, "isMultiSelect", this.isMultiSelect, false);

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];

		if (this.choices.length == 0) {
			result = [{ error: Enums.ValidationError.CollectionCantBeEmpty, message: "An Input.ChoiceSet must have at least one choice defined." }];
		}

		for (var i = 0; i < this.choices.length; i++) {
			if (!this.choices[i].title || !this.choices[i].value) {
				result = result.concat([{ error: Enums.ValidationError.PropertyCantBeNull, message: "All choices in an Input.ChoiceSet must have their title and value properties set." }])
				break;
			}
		}

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.isCompact = !(json["style"] === "expanded");
		this.isMultiSelect = json["isMultiSelect"];
		this.placeholder = json["placeholder"];

		this.choices = [];

		if (json["choices"] != undefined) {
			var choiceArray = json["choices"] as Array<any>;

			for (var i = 0; i < choiceArray.length; i++) {
				var choice = new Choice();

				choice.title = choiceArray[i]["title"];
				choice.value = choiceArray[i]["value"];

				this.choices.push(choice);
			}
		}
	}

	get value(): string {
		if (!this.isMultiSelect) {
			if (this.isCompact) {
				return this._selectElement ? this._selectElement.value : null;
			}
			else {
				if (!this._toggleInputs || this._toggleInputs.length == 0) {
					return null;
				}

				for (var i = 0; i < this._toggleInputs.length; i++) {
					if (this._toggleInputs[i].checked) {
						return this._toggleInputs[i].value;
					}
				}

				return null;
			}
		}
		else {
			if (!this._toggleInputs || this._toggleInputs.length == 0) {
				return null;
			}

			var result: string = "";

			for (var i = 0; i < this._toggleInputs.length; i++) {
				if (this._toggleInputs[i].checked) {
					if (result != "") {
						result += this.hostConfig.choiceSetInputValueSeparator;
					}

					result += this._toggleInputs[i].value;
				}
			}

			return result == "" ? null : result;
		}
	}
}