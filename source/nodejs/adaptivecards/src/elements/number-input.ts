import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";

export class NumberInput extends Core.Input {
	private _numberInputElement: HTMLInputElement;

	protected internalRender(): HTMLElement {
		this._numberInputElement = document.createElement("input");
		this._numberInputElement.setAttribute("type", "number");
		this._numberInputElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-numberInput");
		this._numberInputElement.setAttribute("min", this.min);
		this._numberInputElement.setAttribute("max", this.max);
		this._numberInputElement.style.width = "100%";
		this._numberInputElement.tabIndex = 0;

		if (!Utils.isNullOrEmpty(this.defaultValue)) {
			this._numberInputElement.value = this.defaultValue;
		}

		if (!Utils.isNullOrEmpty(this.placeholder)) {
			this._numberInputElement.placeholder = this.placeholder;
			this._numberInputElement.setAttribute("aria-label", this.placeholder);
		}

		this._numberInputElement.oninput = () => { this.valueChanged(); }

		return this._numberInputElement;
	}

	min: string;
	max: string;
	placeholder: string;

	getJsonTypeName(): string {
		return "Input.Number";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "placeholder", this.placeholder);
		Utils.setProperty(result, "min", this.min);
		Utils.setProperty(result, "max", this.max);

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.placeholder = json["placeholder"];
		this.min = json["min"];
		this.max = json["max"];
	}

	get value(): string {
		return this._numberInputElement ? this._numberInputElement.value : null;
	}
}