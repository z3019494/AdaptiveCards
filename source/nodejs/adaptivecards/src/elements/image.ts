import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as Enums from "../enums";
import * as HostConfig from "../host-config";
import { AdaptiveCard } from "./adaptive-card";

export class Image extends Core.CardElement {
	private _selectAction: Core.Action;

	private parseDimension(name: string, value: any, errors: Array<HostConfig.IValidationError>): number {
		if (value) {
			if (typeof value === "string") {
				try {
					let size = Utils.SizeAndUnit.parse(value);

					if (size.unit == Enums.SizeUnit.Pixel) {
						return size.physicalSize;
					}
				}
				catch {
					// Ignore error
				}
			}

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.InvalidPropertyValue,
					message: "Invalid image " + name + ": " + value
				},
				errors
			);
		}

		return 0;
	}

	private applySize(element: HTMLElement) {
		if (this.pixelWidth || this.pixelHeight) {
			if (this.pixelWidth) {
				element.style.width = this.pixelWidth + "px";
			}

			if (this.pixelHeight) {
				element.style.height = this.pixelHeight + "px";
			}
		}
		else {
			switch (this.size) {
				case Enums.Size.Stretch:
					element.style.width = "100%";
					break;
				case Enums.Size.Auto:
					element.style.maxWidth = "100%";
					break;
				case Enums.Size.Small:
					element.style.width = this.hostConfig.imageSizes.small + "px";
					break;
				case Enums.Size.Large:
					element.style.width = this.hostConfig.imageSizes.large + "px";
					break;
				case Enums.Size.Medium:
					element.style.width = this.hostConfig.imageSizes.medium + "px";
					break;
			}
		}
	}

	protected get useDefaultSizing() {
		return false;
	}

	protected internalRender(): HTMLElement {
		var element: HTMLElement = null;

		if (!Utils.isNullOrEmpty(this.url)) {
			element = document.createElement("div");
			element.style.display = "flex";
			element.style.alignItems = "flex-start";

			element.onkeypress = (e) => {
				if (this.selectAction) {
					if (e.keyCode == 13 || e.keyCode == 32) { // enter or space pressed
						this.selectAction.execute();
					}
				}
			}

			element.onclick = (e) => {
				if (this.selectAction) {
					this.selectAction.execute();
					e.cancelBubble = true;
				}
			}

			switch (this.horizontalAlignment) {
				case Enums.HorizontalAlignment.Center:
					element.style.justifyContent = "center";
					break;
				case Enums.HorizontalAlignment.Right:
					element.style.justifyContent = "flex-end";
					break;
				default:
					element.style.justifyContent = "flex-start";
					break;
			}

			// Cache hostConfig to avoid walking the parent hierarchy multiple times
			let hostConfig = this.hostConfig;

			let imageElement = document.createElement("img");
			imageElement.onload = (e: Event) => {
				Core.CardObject.raiseImageLoadedEvent(this);
			}
			imageElement.onerror = (e: Event) => {
				let card = this.getRootElement() as AdaptiveCard;

				this.renderedElement.innerHTML = "";

				if (card && card.designMode) {
					let errorElement = document.createElement("div");
					errorElement.style.display = "flex";
					errorElement.style.alignItems = "center";
					errorElement.style.justifyContent = "center";
					errorElement.style.backgroundColor = "#EEEEEE";
					errorElement.style.color = "black";
					errorElement.innerText = ":-(";
					errorElement.style.padding = "10px";

					this.applySize(errorElement);

					this.renderedElement.appendChild(errorElement);
				}

				Core.CardObject.raiseImageLoadedEvent(this);
			}
			imageElement.style.maxHeight = "100%";
			imageElement.style.minWidth = "0";
			imageElement.classList.add(hostConfig.makeCssClassName("ac-image"));

			if (this.selectAction != null && hostConfig.supportsInteractivity) {
				imageElement.tabIndex = 0
				imageElement.setAttribute("role", "button");
				imageElement.setAttribute("aria-label", this.selectAction.title);
				imageElement.classList.add(hostConfig.makeCssClassName("ac-selectable"));
			}

			this.applySize(imageElement);

			if (this.style === Enums.ImageStyle.Person) {
				imageElement.style.borderRadius = "50%";
				imageElement.style.backgroundPosition = "50% 50%";
				imageElement.style.backgroundRepeat = "no-repeat";
			}

			if (!Utils.isNullOrEmpty(this.backgroundColor)) {
				imageElement.style.backgroundColor = Utils.stringToCssColor(this.backgroundColor);
			}

			imageElement.src = this.url;
			imageElement.alt = this.altText;

			element.appendChild(imageElement);
		}

		return element;
	}

	style: Enums.ImageStyle = Enums.ImageStyle.Default;
	backgroundColor: string;
	url: string;
	size: Enums.Size = Enums.Size.Auto;
	width: Utils.SizeAndUnit;
	pixelWidth?: number = null;
	pixelHeight?: number = null;
	altText: string = "";

	toJSON() {
		let result = super.toJSON();

		if (this._selectAction) {
			Utils.setProperty(result, "selectAction", this._selectAction.toJSON());
		}

		Utils.setEnumProperty(Enums.ImageStyle, result, "style", this.style, Enums.ImageStyle.Default);
		Utils.setProperty(result, "backgroundColor", this.backgroundColor);
		Utils.setProperty(result, "url", this.url);
		Utils.setEnumProperty(Enums.Size, result, "size", this.size, Enums.Size.Auto);

		if (this.pixelWidth) {
			Utils.setProperty(result, "width", this.pixelWidth + "px");
		}

		if (this.pixelHeight) {
			Utils.setProperty(result, "height", this.pixelHeight + "px");
		}

		Utils.setProperty(result, "altText", this.altText);

		return result;
	}

	getJsonTypeName(): string {
		return "Image";
	}

	getActionById(id: string) {
		var result = super.getActionById(id);

		if (!result && this.selectAction) {
			result = this.selectAction.getActionById(id);
		}

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.url = json["url"];
		this.backgroundColor = json["backgroundColor"];

		var styleString = json["style"];

		if (styleString && typeof styleString === "string" && styleString.toLowerCase() === "normal") {
			this.style = Enums.ImageStyle.Default;

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The Image.style value \"normal\" is deprecated and will be removed. Use \"default\" instead."
				},
				errors
			);
		}
		else {
			this.style = Utils.getEnumValueOrDefault(Enums.ImageStyle, styleString, this.style);
		}

		this.size = Utils.getEnumValueOrDefault(Enums.Size, json["size"], this.size);
		this.altText = json["altText"];

		// pixelWidth and pixelHeight are only parsed for backwards compatibility.
		// Payloads should use the width and height proerties instead.
		if (json["pixelWidth"] && typeof json["pixelWidth"] === "number") {
			this.pixelWidth = json["pixelWidth"];

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The pixelWidth property is deprecated and will be removed. Use the width property instead."
				},
				errors
			);
		}

		if (json["pixelHeight"] && typeof json["pixelHeight"] === "number") {
			this.pixelHeight = json["pixelHeight"];

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The pixelHeight property is deprecated and will be removed. Use the height property instead."
				},
				errors
			);
		}

		let size = this.parseDimension("width", json["width"], errors);

		if (size > 0) {
			this.pixelWidth = size;
		}

		size = this.parseDimension("height", json["height"], errors);

		if (size > 0) {
			this.pixelHeight = size;
		}

		this.selectAction = Core.Action.createActionInstance(
			this,
			json["selectAction"],
			errors);
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		if (!Utils.isNullOrEmpty(this.url)) {
			return [{ url: this.url, mimeType: "image" }]
		}
		else {
			return [];
		}
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak + '\n';
		}

		return null;
	}

	get selectAction(): Core.Action {
		return this._selectAction;
	}

	set selectAction(value: Core.Action) {
		this._selectAction = value;

		if (this._selectAction) {
			this._selectAction.setParent(this);
		}
	}
}