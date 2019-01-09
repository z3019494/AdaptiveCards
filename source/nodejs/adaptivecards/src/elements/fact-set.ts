import * as Core from "../card-elements";
import { TextBlock } from "./text-block";
import * as Utils from "../utils";
import * as Enums from "../enums";
import * as HostConfig from "../host-config";

export class Fact {
	name: string;
	value: string;
	speak: string;

	constructor(name: string = undefined, value: string = undefined) {
		this.name = name;
		this.value = value;
	}

	toJSON() {
		return { title: this.name, value: this.value };
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak + '\n';
		}

		return '<s>' + this.name + ' ' + this.value + '</s>\n';
	}
}

export class FactSet extends Core.CardElement {
	protected get useDefaultSizing(): boolean {
		return false;
	}

	protected internalRender(): HTMLElement {
		let element: HTMLElement = null;
		let hostConfig = this.hostConfig;

		if (this.facts.length > 0) {
			element = document.createElement("table");
			element.style.borderWidth = "0px";
			element.style.borderSpacing = "0px";
			element.style.borderStyle = "none";
			element.style.borderCollapse = "collapse";
			element.style.display = "block";
			element.style.overflow = "hidden";
			element.classList.add(hostConfig.makeCssClassName("ac-factset"));

			for (let i = 0; i < this.facts.length; i++) {
				let trElement = document.createElement("tr");

				if (i > 0) {
					trElement.style.marginTop = this.hostConfig.factSet.spacing + "px";
				}

				// Title column
				let tdElement = document.createElement("td");
				tdElement.style.padding = "0";
				tdElement.classList.add(hostConfig.makeCssClassName("ac-fact-title"));

				if (this.hostConfig.factSet.title.maxWidth) {
					tdElement.style.maxWidth = this.hostConfig.factSet.title.maxWidth + "px";
				}

				tdElement.style.verticalAlign = "top";

				let textBlock = new TextBlock();
				textBlock.setParent(this);
				textBlock.text = Utils.isNullOrEmpty(this.facts[i].name) ? "Title" : this.facts[i].name;
				textBlock.size = this.hostConfig.factSet.title.size;
				textBlock.color = this.hostConfig.factSet.title.color;
				textBlock.isSubtle = this.hostConfig.factSet.title.isSubtle;
				textBlock.weight = this.hostConfig.factSet.title.weight;
				textBlock.wrap = this.hostConfig.factSet.title.wrap;
				textBlock.spacing = Enums.Spacing.None;

				Utils.appendChild(tdElement, textBlock.render());
				Utils.appendChild(trElement, tdElement);

				// Spacer column
				tdElement = document.createElement("td");
				tdElement.style.width = "10px";

				Utils.appendChild(trElement, tdElement);

				// Value column
				tdElement = document.createElement("td");
				tdElement.style.verticalAlign = "top";
				tdElement.classList.add(hostConfig.makeCssClassName("ac-fact-value"));

				textBlock = new TextBlock();
				textBlock.setParent(this);
				textBlock.text = Utils.isNullOrEmpty(this.facts[i].value) ? "Value" : this.facts[i].value;
				textBlock.size = this.hostConfig.factSet.value.size;
				textBlock.color = this.hostConfig.factSet.value.color;
				textBlock.isSubtle = this.hostConfig.factSet.value.isSubtle;
				textBlock.weight = this.hostConfig.factSet.value.weight;
				textBlock.wrap = this.hostConfig.factSet.value.wrap;
				textBlock.spacing = Enums.Spacing.None;

				Utils.appendChild(tdElement, textBlock.render());
				Utils.appendChild(trElement, tdElement);
				Utils.appendChild(element, trElement);
			}
		}

		return element;
	}

	facts: Array<Fact> = [];

	getJsonTypeName(): string {
		return "FactSet";
	}

	toJSON() {
		let result = super.toJSON();

		let facts = []

		for (let fact of this.facts) {
			facts.push(fact.toJSON());
		}

		Utils.setProperty(result, "facts", facts);

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.facts = [];

		if (json["facts"] != null) {
			var jsonFacts = json["facts"] as Array<any>;

			this.facts = [];

			for (var i = 0; i < jsonFacts.length; i++) {
				let fact = new Fact();

				fact.name = jsonFacts[i]["title"];
				fact.value = jsonFacts[i]["value"];
				fact.speak = jsonFacts[i]["speak"];

				this.facts.push(fact);
			}
		}
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak + '\n';
		}

		// render each fact
		let speak = null;

		if (this.facts.length > 0) {
			speak = '';

			for (var i = 0; i < this.facts.length; i++) {
				let speech = this.facts[i].renderSpeech();

				if (speech) {
					speak += speech;
				}
			}
		}

		return '<p>' + speak + '\n</p>\n';
	}
}