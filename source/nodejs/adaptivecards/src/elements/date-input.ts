import * as Core from "../card-elements";
import * as Utils from "../utils";

export class DateInput extends Core.Input {
	private _dateInputElement: HTMLInputElement;

	protected internalRender(): HTMLElement {
		this._dateInputElement = document.createElement("input");
		this._dateInputElement.setAttribute("type", "date");
		this._dateInputElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-dateInput");
		this._dateInputElement.style.width = "100%";

		if (!Utils.isNullOrEmpty(this.defaultValue)) {
			this._dateInputElement.value = this.defaultValue;
		}

		return this._dateInputElement;
	}

	getJsonTypeName(): string {
		return "Input.Date";
	}

	get value(): string {
		return this._dateInputElement ? this._dateInputElement.value : null;
	}
}