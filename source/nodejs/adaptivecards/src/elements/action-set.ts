import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";

export class ActionSet extends Core.CardElement {
	private _actionCollection: Core.ActionCollection;

	protected internalRender(): HTMLElement {
		return this._actionCollection.render(this.orientation ? this.orientation : this.hostConfig.actions.actionsOrientation, this.isDesignMode());
	}

	orientation?: Enums.Orientation = null;

	constructor() {
		super();

		this._actionCollection = new Core.ActionCollection(this);
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setEnumProperty(Enums.Orientation, result, "orientation", this.orientation);
		Utils.setProperty(result, "actions", this._actionCollection.toJSON());

		return result;
	}

	isBleeding(): boolean {
		return this._actionCollection.expandedAction ? true : false;
	}

	getJsonTypeName(): string {
		return "ActionSet";
	}

	getActionCount(): number {
		return this._actionCollection.items.length;
	}

	getActionAt(index: number): Core.Action {
		if (index >= 0 && index < this.getActionCount()) {
			return this._actionCollection.items[index];
		}
		else {
			super.getActionAt(index);
		}
	}

	validate(): Array<HostConfig.IValidationError> {
		return this._actionCollection.validate();
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		var jsonOrientation = json["orientation"];

		if (jsonOrientation) {
			this.orientation = Utils.getEnumValueOrDefault(Enums.Orientation, jsonOrientation, Enums.Orientation.Horizontal);
		}

		this._actionCollection.parse(json["actions"], errors);
	}

	addAction(action: Core.Action) {
		this._actionCollection.addAction(action);
	}

	getAllInputs(): Array<Core.Input> {
		return this._actionCollection.getAllInputs();
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		return this._actionCollection.getResourceInformation();
	}

	renderSpeech(): string {
		// TODO: What's the right thing to do here?
		return "";
	}

	get isInteractive(): boolean {
		return true;
	}
}