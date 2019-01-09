import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";

export class OpenUrlAction extends Core.Action {
	url: string;

	getJsonTypeName(): string {
		return "Action.OpenUrl";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "url", this.url);

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		if (!this.url) {
			return [{ error: Enums.ValidationError.PropertyCantBeNull, message: "An Action.OpenUrl must have its url property set." }];
		}
		else {
			return [];
		}
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.url = json["url"];
	}
}