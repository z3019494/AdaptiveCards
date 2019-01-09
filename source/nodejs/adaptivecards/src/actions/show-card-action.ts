import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import { AdaptiveCard, InlineAdaptiveCard } from "../elements/adaptive-card";

export class ShowCardAction extends Core.Action {
	static onInlineCardExpanded: (action: ShowCardAction, isExpanded: boolean) => void = null;

    protected addCssClasses(element: HTMLElement) {
		super.addCssClasses(element);

		element.classList.add(this.parent.hostConfig.makeCssClassName("expandable"));
	}

    readonly card: AdaptiveCard = new InlineAdaptiveCard();
    
    expand(suppressStyle: boolean): HTMLElement {
		(<InlineAdaptiveCard>this.card).suppressStyle = suppressStyle;

		return this.card.render();
    }

	getJsonTypeName(): string {
		return "Action.ShowCard";
	}

	toJSON() {
		let result = super.toJSON();

		if (this.card) {
			Utils.setProperty(result, "card", this.card.toJSON());
		}

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		return this.card.validate();
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.card.parse(json["card"], errors);
	}

	setParent(value: Core.CardElement) {
		super.setParent(value);

		this.card.setParent(value);
	}

	getAllInputs(): Array<Core.Input> {
		return this.card.getAllInputs();
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		return super.getResourceInformation().concat(this.card.getResourceInformation());
	}

	getActionById(id: string): Core.Action {
		var result = super.getActionById(id);

		if (!result) {
			result = this.card.getActionById(id);
		}

		return result;
	}

	get isExpandable(): boolean {
		return true;
    }
}