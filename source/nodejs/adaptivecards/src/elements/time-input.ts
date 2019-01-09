import * as Core from "../card-elements";
import * as Utils from "../utils";

export class TimeInput extends Core.Input {
	private _timeInputElement: HTMLInputElement;

	protected internalRender(): HTMLElement {
		this._timeInputElement = document.createElement("input");
		this._timeInputElement.setAttribute("type", "time");
		this._timeInputElement.className = this.hostConfig.makeCssClassName("ac-input", "ac-timeInput");
		this._timeInputElement.style.width = "100%";

		if (!Utils.isNullOrEmpty(this.defaultValue)) {
			this._timeInputElement.value = this.defaultValue;
		}

		return this._timeInputElement;
	}

	getJsonTypeName(): string {
		return "Input.Time";
	}

	get value(): string {
		return this._timeInputElement ? this._timeInputElement.value : null;
	}
}