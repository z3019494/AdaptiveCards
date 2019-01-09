import * as Core from "../card-elements";
import * as Containers from "./container";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";
// import { Image } from "./image";
// import { ShowCardAction } from "../actions/show-card-action";

export class AdaptiveCard extends Containers.ContainerWithActions {
	private static currentVersion: HostConfig.Version = new HostConfig.Version(1, 1);

	private _cardTypeName?: string = "AdaptiveCard";
	private _fallbackCard: AdaptiveCard = null;

	private isVersionSupported(): boolean {
		if (this.bypassVersionCheck) {
			return true;
		}
		else {
			let unsupportedVersion: boolean =
				!this.version ||
				!this.version.isValid ||
				(AdaptiveCard.currentVersion.major < this.version.major) ||
				(AdaptiveCard.currentVersion.major == this.version.major && AdaptiveCard.currentVersion.minor < this.version.minor);

			return !unsupportedVersion;
		}
	}

	protected get renderIfEmpty(): boolean {
		return true;
	}

	protected getItemsCollectionPropertyName(): string {
		return "body";
	}

	protected applyPadding() {
		if (!this.renderedElement) {
			return;
		}

		var effectivePadding = this.padding ? this.padding.toSpacingDefinition(this.hostConfig) : this.internalPadding.toSpacingDefinition(this.hostConfig);

		this.renderedElement.style.paddingTop = effectivePadding.top + "px";
		this.renderedElement.style.paddingRight = effectivePadding.right + "px";
		this.renderedElement.style.paddingBottom = effectivePadding.bottom + "px";
		this.renderedElement.style.paddingLeft = effectivePadding.left + "px";

		if (this.isLastElementBleeding()) {
			this.renderedElement.style.paddingBottom = "0px";
		}
	}

	protected internalRender(): HTMLElement {
		var renderedElement = super.internalRender();

		if (AdaptiveCard.useAdvancedCardBottomTruncation) {
			// Unlike containers, the root card element should be allowed to
			// be shorter than its content (otherwise the overflow truncation
			// logic would never get triggered)
			renderedElement.style.minHeight = null;
		}

		return renderedElement;
	}

	protected get bypassVersionCheck(): boolean {
		return false;
	}

	protected get defaultPadding(): Core.PaddingDefinition {
		return new Core.PaddingDefinition(
			Enums.Spacing.Padding,
			Enums.Spacing.Padding,
			Enums.Spacing.Padding,
			Enums.Spacing.Padding);
	}

	protected get allowCustomPadding(): boolean {
		return false;
	}

	protected get allowCustomStyle() {
		return this.hostConfig.adaptiveCard && this.hostConfig.adaptiveCard.allowCustomStyle;
	}

	protected get hasBackground(): boolean {
		return true;
	}

	protected isDesignMode(): boolean {
		return this.designMode;
	}

	version?: HostConfig.Version = new HostConfig.Version(1, 0);
	fallbackText: string;
	designMode: boolean = false;

	getJsonTypeName(): string {
		return "AdaptiveCard";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "$schema", "http://adaptivecards.io/schemas/adaptive-card.json");

		if (!this.bypassVersionCheck && this.version) {
			Utils.setProperty(result, "version", this.version.toString());
		}

		Utils.setProperty(result, "fallbackText", this.fallbackText);
		Utils.setProperty(result, "lang", this.lang);
		Utils.setProperty(result, "speak", this.speak);

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];

		if (this._cardTypeName != "AdaptiveCard") {
			result.push(
				{
					error: Enums.ValidationError.MissingCardType,
					message: "Invalid or missing card type. Make sure the card's type property is set to \"AdaptiveCard\"."
				});
		}

		if (!this.bypassVersionCheck && !this.version) {
			result.push(
				{
					error: Enums.ValidationError.PropertyCantBeNull,
					message: "The version property must be specified."
				});
		}
		else if (!this.isVersionSupported()) {
			result.push(
				{
					error: Enums.ValidationError.UnsupportedCardVersion,
					message: "The specified card version (" + this.version + ") is not supported. The maximum supported card version is " + AdaptiveCard.currentVersion
				});
		}

		return result.concat(super.validate());
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		this._fallbackCard = null;

		this._cardTypeName = json["type"];

		var langId = json["lang"];

		if (langId && typeof langId === "string") {
			try {
				this.lang = langId;
			}
			catch (e) {
				Core.CardObject.raiseParseError(
					{
						error: Enums.ValidationError.InvalidPropertyValue,
						message: e.message
					},
					errors
				);
			}
		}

		this.version = HostConfig.Version.parse(json["version"], errors);

		this.fallbackText = json["fallbackText"];

		let fallbackElement = Core.CardElement.createCardElementInstance(null, json["fallback"], errors);

		if (fallbackElement) {
			this._fallbackCard = new AdaptiveCard();
			this._fallbackCard.addItem(fallbackElement);
		}

		super.parse(json, errors);
	}

	render(target?: HTMLElement): HTMLElement {
		let fallback = false;
		let renderedCard: HTMLElement;

		if (this.shouldFallback()) {
			if (this._fallbackCard) {
				this._fallbackCard.hostConfig = this.hostConfig;

				renderedCard = this._fallbackCard.render();
			}
			else {
				let errorText = !Utils.isNullOrEmpty(this.fallbackText) ? this.fallbackText : "The card could not be rendered. It is either malformed or uses features not supported by this host.";

				try {
					let fallbackCard = new AdaptiveCard();
					fallbackCard.hostConfig = this.hostConfig;
					fallbackCard.parse(
						{
							type: "AdaptiveCard",
							version: "1.0",
							body: [
								{
									type: "TextBlock",
									text: errorText,
									wrap: true
								}
							]
						});

					renderedCard = fallbackCard.render();
				}
				catch (e) {
					renderedCard = document.createElement("div");
					renderedCard.innerHTML = errorText;
				}
			}
		}
		else {
			renderedCard = super.render();

			if (renderedCard) {
				renderedCard.tabIndex = 0;

				if (!Utils.isNullOrEmpty(this.speak)) {
					renderedCard.setAttribute("aria-label", this.speak);
				}
			}
		}

		if (target) {
			target.appendChild(renderedCard);

			this.updateLayout();
		}

		return renderedCard;
	}

	updateLayout(processChildren: boolean = true) {
		super.updateLayout(processChildren);

		if (AdaptiveCard.useAdvancedCardBottomTruncation && this.isRendered()) {
			var card = this.renderedElement;
			var padding = this.hostConfig.getEffectiveSpacing(Enums.Spacing.Default);

			this['handleOverflow'](card.offsetHeight - padding);
		}
	}

	shouldFallback(): boolean {
		return super.shouldFallback() || !this.isVersionSupported();
	}

	get hasVisibleSeparator(): boolean {
		return false;
	}
}

export class InlineAdaptiveCard extends AdaptiveCard {
	protected get bypassVersionCheck(): boolean {
		return true;
	}

	protected get defaultPadding(): Core.PaddingDefinition {
		return new Core.PaddingDefinition(
			this.suppressStyle ? Enums.Spacing.None : Enums.Spacing.Padding,
			Enums.Spacing.Padding,
			this.suppressStyle ? Enums.Spacing.None : Enums.Spacing.Padding,
			Enums.Spacing.Padding);
	}

	protected get defaultStyle(): string {
		if (this.suppressStyle) {
			return Enums.ContainerStyle.Default;
		}
		else {
			return this.hostConfig.actions.showCard.style ? this.hostConfig.actions.showCard.style : Enums.ContainerStyle.Emphasis;
		}
	}

	suppressStyle: boolean = false;

	render(target?: HTMLElement) {
		var renderedCard = super.render(target);
		renderedCard.setAttribute("aria-live", "polite");
		renderedCard.removeAttribute("tabindex");

		return renderedCard;
	}
}