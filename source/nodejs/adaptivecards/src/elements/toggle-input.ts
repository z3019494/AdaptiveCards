import * as Core from "../card-elements";
import { Label } from "./label";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import { AdaptiveCard } from "./adaptive-card";

export class ToggleInput extends Core.Input {
	private _checkboxInputElement: HTMLInputElement;

	protected internalRender(): HTMLElement {
		let element = document.createElement("div");
		element.className = this.hostConfig.makeCssClassName("ac-input");
		element.style.width = "100%";
		element.style.display = "flex";
		element.style.alignItems = "center";

		this._checkboxInputElement = document.createElement("input");
		this._checkboxInputElement.id = Utils.generateUniqueId();
		this._checkboxInputElement.type = "checkbox";
		this._checkboxInputElement.style.display = "inline-block";
		this._checkboxInputElement.style.verticalAlign = "middle";
		this._checkboxInputElement.style.margin = "0";
		this._checkboxInputElement.style.flex = "0 0 auto";
		this._checkboxInputElement.setAttribute("aria-label", this.title);
		this._checkboxInputElement.tabIndex = 0;

		if (this.defaultValue == this.valueOn) {
			this._checkboxInputElement.checked = true;
		}

		this._checkboxInputElement.onchange = () => { this.valueChanged(); }

		Utils.appendChild(element, this._checkboxInputElement);

		if (!Utils.isNullOrEmpty(this.title) || this.isDesignMode()) {
			let label = new Label();
			label.setParent(this);
			label.forElementId = this._checkboxInputElement.id;
			label.hostConfig = this.hostConfig;
			label.text = Utils.isNullOrEmpty(this.title) ? this.getJsonTypeName() : this.title;
			label.useMarkdown = AdaptiveCard.useMarkdownInRadioButtonAndCheckbox;

			let labelElement = label.render();
			labelElement.style.display = "inline-block";
			labelElement.style.flex = "1 1 auto";
			labelElement.style.verticalAlign = "middle";

			let spacerElement = document.createElement("div");
			spacerElement.style.width = "6px";

			Utils.appendChild(element, spacerElement);
			Utils.appendChild(element, labelElement);
		}

		return element;
	}

	valueOn: string = "true";
	valueOff: string = "false";

	getJsonTypeName(): string {
		return "Input.Toggle";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "valueOn", this.valueOn, "true");
		Utils.setProperty(result, "valueOff", this.valueOff, "false");

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.title = json["title"];

		this.valueOn = Utils.getValueOrDefault<string>(json["valueOn"], this.valueOn);
		this.valueOff = Utils.getValueOrDefault<string>(json["valueOff"], this.valueOff);
	}

	get value(): string {
		if (this._checkboxInputElement) {
			return this._checkboxInputElement.checked ? this.valueOn : this.valueOff;
		}
		else {
			return null;
		}
	}
}