import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";

export class SubmitAction extends Core.Action {
	private _isPrepared: boolean = false;
	private _originalData: Object;
	private _processedData: Object;

	getJsonTypeName(): string {
		return "Action.Submit";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "data", this._originalData);

		return result;
	}

	prepare(inputs: Array<Core.Input>) {
		if (this._originalData) {
			this._processedData = JSON.parse(JSON.stringify(this._originalData));
		}
		else {
			this._processedData = {};
		}

		for (var i = 0; i < inputs.length; i++) {
			var inputValue = inputs[i].value;

			if (inputValue != null) {
				this._processedData[inputs[i].id] = inputs[i].value;
			}
		}

		this._isPrepared = true;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.data = json["data"];
	}

	get data(): Object {
		return this._isPrepared ? this._processedData : this._originalData;
	}

	set data(value: Object) {
		this._originalData = value;
		this._isPrepared = false;
	}
}