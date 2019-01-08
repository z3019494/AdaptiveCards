import { Action, OpenUrlAction, SubmitAction, ShowCardAction, CardElement, Container, TextBlock, Image, ImageSet, Media, FactSet, TextInput, DateInput, TimeInput, NumberInput, ChoiceSetInput, ToggleInput } from "./card-elements";
import { ColumnSet } from "./elements/column-set";

export interface ITypeRegistration<T> {
	typeName: string,
	createInstance: () => T;
}

export abstract class TypeRegistry<T> {
	private _items: Array<ITypeRegistration<T>> = [];

	private findTypeRegistration(typeName: string): ITypeRegistration<T> {
		for (var i = 0; i < this._items.length; i++) {
			if (this._items[i].typeName === typeName) {
				return this._items[i];
			}
		}

		return null;
	}

	constructor() {
		this.reset();
	}

	clear() {
		this._items = [];
	}

	abstract reset();

	registerType(typeName: string, createInstance: () => T) {
		var registrationInfo = this.findTypeRegistration(typeName);

		if (registrationInfo != null) {
			registrationInfo.createInstance = createInstance;
		}
		else {
			registrationInfo = {
				typeName: typeName,
				createInstance: createInstance
			}

			this._items.push(registrationInfo);
		}
	}

	unregisterType(typeName: string) {
		for (var i = 0; i < this._items.length; i++) {
			if (this._items[i].typeName === typeName) {
				this._items.splice(i, 1);

				return;
			}
		}
	}

	createInstance(typeName: string): T {
		var registrationInfo = this.findTypeRegistration(typeName);

		return registrationInfo ? registrationInfo.createInstance() : null;
	}

	getItemCount(): number {
		return this._items.length;
	}

	getItemAt(index: number): ITypeRegistration<T> {
		return this._items[index];
	}
}

export class ElementTypeRegistry extends TypeRegistry<CardElement> {
	reset() {
		this.clear();

		this.registerType("Container", () => { return new Container(); });
		this.registerType("TextBlock", () => { return new TextBlock(); });
		this.registerType("Image", () => { return new Image(); });
		this.registerType("ImageSet", () => { return new ImageSet(); });
		this.registerType("Media", () => { return new Media(); });
		this.registerType("FactSet", () => { return new FactSet(); });
		this.registerType("ColumnSet", () => { return new ColumnSet(); });
		this.registerType("Input.Text", () => { return new TextInput(); });
		this.registerType("Input.Date", () => { return new DateInput(); });
		this.registerType("Input.Time", () => { return new TimeInput(); });
		this.registerType("Input.Number", () => { return new NumberInput(); });
		this.registerType("Input.ChoiceSet", () => { return new ChoiceSetInput(); });
		this.registerType("Input.Toggle", () => { return new ToggleInput(); });
	}
}

export class ActionTypeRegistry extends TypeRegistry<Action> {
	reset() {
		this.clear();

		this.registerType("Action.OpenUrl", () => { return new OpenUrlAction(); });
		this.registerType("Action.Submit", () => { return new SubmitAction(); });
		this.registerType("Action.ShowCard", () => { return new ShowCardAction(); });
	}
}