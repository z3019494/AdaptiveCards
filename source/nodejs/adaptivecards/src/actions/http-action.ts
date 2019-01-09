import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";

export class HttpHeader {
	private _value = new Utils.StringWithSubstitutions();

	name: string;

	constructor(name: string = "", value: string = "") {
		this.name = name;
		this.value = value;
	}

	toJSON() {
		return { name: this.name, value: this._value.getOriginal() };
	}

	prepare(inputs: Array<Core.Input>) {
		this._value.substituteInputValues(inputs, Utils.ContentTypes.applicationXWwwFormUrlencoded);
	}

	get value(): string {
		return this._value.get();
	}

	set value(newValue: string) {
		this._value.set(newValue);
	}
}

export class HttpAction extends Core.Action {
	private _url = new Utils.StringWithSubstitutions();
	private _body = new Utils.StringWithSubstitutions();
	private _headers: Array<HttpHeader> = [];

	method: string;

	getJsonTypeName(): string {
		return "Action.Http";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "method", this.method);
		Utils.setProperty(result, "url", this._url.getOriginal());
		Utils.setProperty(result, "body", this._body.getOriginal());

		if (this._headers.length > 0) {
			let headers = [];

			for (let header of this._headers) {
				headers.push(header.toJSON());
			}

			Utils.setProperty(result, "headers", headers);
		}

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];

		if (!this.url) {
			result = [{ error: Enums.ValidationError.PropertyCantBeNull, message: "An Action.Http must have its url property set." }];
		}

		if (this.headers.length > 0) {
			for (var i = 0; i < this.headers.length; i++) {
				if (!this.headers[i].name || !this.headers[i].value) {
					result = result.concat([{ error: Enums.ValidationError.PropertyCantBeNull, message: "All headers of an Action.Http must have their name and value properties set." }]);
					break;
				}
			}
		}

		return result;
	}

	prepare(inputs: Array<Core.Input>) {
		this._url.substituteInputValues(inputs, Utils.ContentTypes.applicationXWwwFormUrlencoded);

		let contentType = Utils.ContentTypes.applicationJson;

		for (var i = 0; i < this._headers.length; i++) {
			this._headers[i].prepare(inputs);

			if (this._headers[i].name && this._headers[i].name.toLowerCase() == "content-type") {
				contentType = this._headers[i].value;
			}
		}

		this._body.substituteInputValues(inputs, contentType);
	};

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.url = json["url"];
		this.method = json["method"];
		this.body = json["body"];

		this._headers = [];

		if (json["headers"] != null) {
			var jsonHeaders = json["headers"] as Array<any>;

			for (var i = 0; i < jsonHeaders.length; i++) {
				let httpHeader = new HttpHeader();

				httpHeader.name = jsonHeaders[i]["name"];
				httpHeader.value = jsonHeaders[i]["value"];

				this.headers.push(httpHeader);
			}
		}
	}

	get url(): string {
		return this._url.get();
	}

	set url(value: string) {
		this._url.set(value);
	}

	get body(): string {
		return this._body.get();
	}

	set body(value: string) {
		this._body.set(value);
	}

	get headers(): Array<HttpHeader> {
		return this._headers ? this._headers : [];
	}

	set headers(value: Array<HttpHeader>) {
		this._headers = value;
	}
}