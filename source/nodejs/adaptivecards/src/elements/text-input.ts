import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as Enums from "../enums";
import * as HostConfig from "../host-config";

export class TextInput extends Core.Input {
	private _textareaElement: HTMLTextAreaElement;
	private _inputElement: HTMLInputElement;

	protected internalRender(): HTMLElement {
		if (this.isMultiline) {
			this._textareaElement = document.createElement("textarea");
			this._textareaElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-textInput", "ac-multiline");
			this._textareaElement.style.width = "100%";
			this._textareaElement.tabIndex = 0;

			if (!Utils.isNullOrEmpty(this.placeholder)) {
				this._textareaElement.placeholder = this.placeholder;
				this._textareaElement.setAttribute("aria-label", this.placeholder)
			}

			if (!Utils.isNullOrEmpty(this.defaultValue)) {
				this._textareaElement.value = this.defaultValue;
			}

			if (this.maxLength > 0) {
				this._textareaElement.maxLength = this.maxLength;
			}

			this._textareaElement.oninput = () => { this.valueChanged(); }

			return this._textareaElement;
		}
		else {
			this._inputElement = document.createElement("input");
			this._inputElement.type = Enums.InputTextStyle[this.style].toLowerCase();
			this._inputElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-textInput");
			this._inputElement.style.width = "100%";
			this._inputElement.tabIndex = 0;

			if (!Utils.isNullOrEmpty(this.placeholder)) {
				this._inputElement.placeholder = this.placeholder;
				this._inputElement.setAttribute("aria-label", this.placeholder)
			}

			if (!Utils.isNullOrEmpty(this.defaultValue)) {
				this._inputElement.value = this.defaultValue;
			}

			if (this.maxLength > 0) {
				this._inputElement.maxLength = this.maxLength;
			}

			this._inputElement.oninput = () => { this.valueChanged(); }

			return this._inputElement;
		}
	}

	maxLength: number;
	isMultiline: boolean;
	placeholder: string;
	style: Enums.InputTextStyle = Enums.InputTextStyle.Text;

	getJsonTypeName(): string {
		return "Input.Text";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "placeholder", this.placeholder);
		Utils.setProperty(result, "maxLength", this.maxLength, 0);
		Utils.setProperty(result, "isMultiline", this.isMultiline, false);
		Utils.setEnumProperty(Enums.InputTextStyle, result, "style", this.style, Enums.InputTextStyle.Text);

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.maxLength = json["maxLength"];
		this.isMultiline = json["isMultiline"];
		this.placeholder = json["placeholder"];
		this.style = Utils.getEnumValueOrDefault(Enums.InputTextStyle, json["style"], this.style);
	}

	get value(): string {
		if (this.isMultiline) {
			return this._textareaElement ? this._textareaElement.value : null;
		}
		else {
			return this._inputElement ? this._inputElement.value : null;
		}
	}
}