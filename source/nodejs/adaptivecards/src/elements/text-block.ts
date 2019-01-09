import * as Core from "../card-elements";
import * as HostConfig from "../host-config";
import * as TextFormatters from "../text-formatters";
import * as Enums from "../enums";
import * as Utils from "../utils";
import { AdaptiveCard } from "./adaptive-card";

export class TextBlock extends Core.CardElement {
	private _computedLineHeight: number;
	private _originalInnerHtml: string;
	private _text: string;
	private _processedText: string = null;
	private _treatAsPlainText: boolean = true;
	private _selectAction: Core.Action = null;
	private _effectiveStyleDefinition: HostConfig.ContainerStyleDefinition = null;

	private restoreOriginalContent() {
		var maxHeight = this.maxLines
			? (this._computedLineHeight * this.maxLines) + 'px'
			: null;

		this.renderedElement.style.maxHeight = maxHeight;
		this.renderedElement.innerHTML = this._originalInnerHtml;
	}

	private truncateIfSupported(maxHeight: number): boolean {
		// For now, only truncate TextBlocks that contain just a single
		// paragraph -- since the maxLines calculation doesn't take into
		// account Markdown lists
		var children = this.renderedElement.children;
		var isTextOnly = !children.length;

		var truncationSupported = isTextOnly || children.length == 1
			&& (<HTMLElement>children[0]).tagName.toLowerCase() == 'p';

		if (truncationSupported) {
			var element = isTextOnly
				? this.renderedElement
				: <HTMLElement>children[0];

			Utils.truncate(element, maxHeight, this._computedLineHeight);
			return true;
		}

		return false;
	}

	private getEffectiveStyleDefinition() {
		if (!this._effectiveStyleDefinition) {
			this._effectiveStyleDefinition = this.hostConfig.containerStyles.default;

			let parentContainer = this.getParentContainer();

			while (parentContainer) {
				if (parentContainer.style) {
					this._effectiveStyleDefinition = this.hostConfig.containerStyles.getStyleByName(parentContainer.style);

					break;
				}

				parentContainer = parentContainer.getParentContainer();
			}
		}

		return this._effectiveStyleDefinition;
	}

	protected getRenderedDomElementType(): string {
		return "div";
	}

	protected internalRender(): HTMLElement {
		this._effectiveStyleDefinition = null;
		this._processedText = null;

		if (!Utils.isNullOrEmpty(this.text)) {
			let hostConfig = this.hostConfig;

			let element = document.createElement(this.getRenderedDomElementType());
			element.classList.add(hostConfig.makeCssClassName("ac-textBlock"));
			element.style.overflow = "hidden";

			this.applyStylesTo(element);

			if (this.selectAction) {
				element.onclick = (e) => {
					this.selectAction.execute();

					e.cancelBubble = true;
				}
			}

			if (!this._processedText) {
				this._treatAsPlainText = true;

				let formattedText = TextFormatters.formatText(this.lang, this.text);

				if (this.useMarkdown) {
					if (AdaptiveCard.allowMarkForTextHighlighting) {
						formattedText = formattedText.replace(/<mark>/g, "===").replace(/<\/mark>/g, "/==");
					}

					let markdownProcessingResult = AdaptiveCard.applyMarkdown(formattedText);

					if (markdownProcessingResult.didProcess && markdownProcessingResult.outputHtml) {
						this._processedText = markdownProcessingResult.outputHtml;
						this._treatAsPlainText = false;

						// Only process <mark> tag if markdown processing was applied because
						// markdown processing is also responsible for sanitizing the input string
						if (AdaptiveCard.allowMarkForTextHighlighting) {
							let markStyle: string = "";
							let effectiveStyle = this.getEffectiveStyleDefinition();

							if (effectiveStyle.highlightBackgroundColor) {
								markStyle += "background-color: " + effectiveStyle.highlightBackgroundColor + ";";
							}

							if (effectiveStyle.highlightForegroundColor) {
								markStyle += "color: " + effectiveStyle.highlightForegroundColor + ";";
							}

							if (!Utils.isNullOrEmpty(markStyle)) {
								markStyle = 'style="' + markStyle + '"';
							}

							this._processedText = this._processedText.replace(/===/g, "<mark " + markStyle + ">").replace(/\/==/g, "</mark>");
						}
					} else {
						this._processedText = formattedText;
						this._treatAsPlainText = true;
					}
				}
				else {
					this._processedText = formattedText;
					this._treatAsPlainText = true;
				}
			}

			if (this._treatAsPlainText) {
				element.innerText = this._processedText;
			}
			else {
				element.innerHTML = this._processedText;
			}

			if (element.firstElementChild instanceof HTMLElement) {
				let firstElementChild = <HTMLElement>element.firstElementChild;
				firstElementChild.style.marginTop = "0px";
				firstElementChild.style.width = "100%";

				if (!this.wrap) {
					firstElementChild.style.overflow = "hidden";
					firstElementChild.style.textOverflow = "ellipsis";
				}
			}

			if (element.lastElementChild instanceof HTMLElement) {
				(<HTMLElement>element.lastElementChild).style.marginBottom = "0px";
			}

			let anchors = element.getElementsByTagName("a");

			for (let i = 0; i < anchors.length; i++) {
				let anchor = <HTMLAnchorElement>anchors[i];
				anchor.classList.add(this.hostConfig.makeCssClassName("ac-anchor"));
				anchor.target = "_blank";
				anchor.onclick = (e) => {
					if (Core.CardObject.raiseAnchorClickedEvent(this, e.target as HTMLAnchorElement)) {
						e.preventDefault();
					}
				}
			}

			if (this.wrap) {
				element.style.wordWrap = "break-word";

				if (this.maxLines > 0) {
					element.style.maxHeight = (this._computedLineHeight * this.maxLines) + "px";
					element.style.overflow = "hidden";
				}
			}
			else {
				element.style.whiteSpace = "nowrap";
				element.style.textOverflow = "ellipsis";
			}

			if (AdaptiveCard.useAdvancedTextBlockTruncation || AdaptiveCard.useAdvancedCardBottomTruncation) {
				this._originalInnerHtml = element.innerHTML;
			}

			if (this.selectAction != null && hostConfig.supportsInteractivity) {
				element.tabIndex = 0
				element.setAttribute("role", "button");
				element.setAttribute("aria-label", this.selectAction.title);
				element.classList.add(hostConfig.makeCssClassName("ac-selectable"));
			}

			return element;
		}
		else {
			return null;
		}
	}

	protected truncateOverflow(maxHeight: number): boolean {
		if (maxHeight >= this._computedLineHeight) {
			return this.truncateIfSupported(maxHeight);
		}

		return false;
	}

	protected undoOverflowTruncation() {
		this.restoreOriginalContent();

		if (AdaptiveCard.useAdvancedTextBlockTruncation && this.maxLines) {
			var maxHeight = this._computedLineHeight * this.maxLines;
			this.truncateIfSupported(maxHeight);
		}
	}

	size: Enums.TextSize = Enums.TextSize.Default;
	weight: Enums.TextWeight = Enums.TextWeight.Default;
	color: Enums.TextColor = Enums.TextColor.Default;
	isSubtle: boolean = false;
	wrap: boolean = false;
	maxLines: number;
	useMarkdown: boolean = true;

	toJSON() {
		let result = super.toJSON();

		Utils.setEnumProperty(Enums.TextSize, result, "size", this.size, Enums.TextSize.Default);
		Utils.setEnumProperty(Enums.TextWeight, result, "weight", this.weight, Enums.TextWeight.Default);
		Utils.setEnumProperty(Enums.TextColor, result, "color", this.color, Enums.TextColor.Default);
		Utils.setProperty(result, "text", this.text);
		Utils.setProperty(result, "isSubtle", this.isSubtle, false);
		Utils.setProperty(result, "wrap", this.wrap, false);
		Utils.setProperty(result, "maxLines", this.maxLines, 0);

		return result;
	}

	applyStylesTo(targetElement: HTMLElement) {
		if (this.hostConfig.fontFamily) {
			targetElement.style.fontFamily = this.hostConfig.fontFamily;
		}

		let parentContainer = this.getParentContainer();
		let isRtl = parentContainer ? parentContainer.isRtl() : false;

		switch (this.horizontalAlignment) {
			case Enums.HorizontalAlignment.Center:
				targetElement.style.textAlign = "center";
				break;
			case Enums.HorizontalAlignment.Right:
				targetElement.style.textAlign = isRtl ? "left" : "right";
				break;
			default:
				targetElement.style.textAlign = isRtl ? "right" : "left";
				break;
		}

		var fontSize: number;

		switch (this.size) {
			case Enums.TextSize.Small:
				fontSize = this.hostConfig.fontSizes.small;
				break;
			case Enums.TextSize.Medium:
				fontSize = this.hostConfig.fontSizes.medium;
				break;
			case Enums.TextSize.Large:
				fontSize = this.hostConfig.fontSizes.large;
				break;
			case Enums.TextSize.ExtraLarge:
				fontSize = this.hostConfig.fontSizes.extraLarge;
				break;
			default:
				fontSize = this.hostConfig.fontSizes.default;
				break;
		}

		if (this.hostConfig.lineHeights) {
			switch (this.size) {
				case Enums.TextSize.Small:
					this._computedLineHeight = this.hostConfig.lineHeights.small;
					break;
				case Enums.TextSize.Medium:
					this._computedLineHeight = this.hostConfig.lineHeights.medium;
					break;
				case Enums.TextSize.Large:
					this._computedLineHeight = this.hostConfig.lineHeights.large;
					break;
				case Enums.TextSize.ExtraLarge:
					this._computedLineHeight = this.hostConfig.lineHeights.extraLarge;
					break;
				default:
					this._computedLineHeight = this.hostConfig.lineHeights.default;
					break;
			}
		}
		else {
			// Looks like 1.33 is the magic number to compute line-height
			// from font size.
			this._computedLineHeight = fontSize * 1.33;
		}

		targetElement.style.fontSize = fontSize + "px";
		targetElement.style.lineHeight = this._computedLineHeight + "px";

		let styleDefinition = this.getEffectiveStyleDefinition();

		let actualTextColor = this.color ? this.color : Enums.TextColor.Default;
		let colorDefinition: HostConfig.TextColorDefinition;

		switch (actualTextColor) {
			case Enums.TextColor.Accent:
				colorDefinition = styleDefinition.foregroundColors.accent;
				break;
			case Enums.TextColor.Dark:
				colorDefinition = styleDefinition.foregroundColors.dark;
				break;
			case Enums.TextColor.Light:
				colorDefinition = styleDefinition.foregroundColors.light;
				break;
			case Enums.TextColor.Good:
				colorDefinition = styleDefinition.foregroundColors.good;
				break;
			case Enums.TextColor.Warning:
				colorDefinition = styleDefinition.foregroundColors.warning;
				break;
			case Enums.TextColor.Attention:
				colorDefinition = styleDefinition.foregroundColors.attention;
				break;
			default:
				colorDefinition = styleDefinition.foregroundColors.default;
				break;
		}

		targetElement.style.color = Utils.stringToCssColor(this.isSubtle ? colorDefinition.subtle : colorDefinition.default);

		let fontWeight: number;

		switch (this.weight) {
			case Enums.TextWeight.Lighter:
				fontWeight = this.hostConfig.fontWeights.lighter;
				break;
			case Enums.TextWeight.Bolder:
				fontWeight = this.hostConfig.fontWeights.bolder;
				break;
			default:
				fontWeight = this.hostConfig.fontWeights.default;
				break;
		}

		targetElement.style.fontWeight = fontWeight.toString();
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.text = json["text"];

		var sizeString = json["size"];

		if (sizeString && typeof sizeString === "string" && sizeString.toLowerCase() === "normal") {
			this.size = Enums.TextSize.Default;

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The TextBlock.size value \"normal\" is deprecated and will be removed. Use \"default\" instead."
				},
				errors
			);
		}
		else {
			this.size = Utils.getEnumValueOrDefault(Enums.TextSize, sizeString, this.size);
		}

		var weightString = json["weight"];

		if (weightString && typeof weightString === "string" && weightString.toLowerCase() === "normal") {
			this.weight = Enums.TextWeight.Default;

			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The TextBlock.weight value \"normal\" is deprecated and will be removed. Use \"default\" instead."
				},
				errors
			);
		}
		else {
			this.weight = Utils.getEnumValueOrDefault(Enums.TextWeight, weightString, this.weight);
		}

		this.color = Utils.getEnumValueOrDefault(Enums.TextColor, json["color"], this.color);
		this.isSubtle = json["isSubtle"];
		this.wrap = json["wrap"] === undefined ? false : json["wrap"];

		if (typeof json["maxLines"] === "number") {
			this.maxLines = json["maxLines"];
		}
	}

	getJsonTypeName(): string {
		return "TextBlock";
	}

	renderSpeech(): string {
		if (this.speak != null)
			return this.speak + '\n';

		if (this.text)
			return '<s>' + this.text + '</s>\n';

		return null;
	}

	updateLayout(processChildren: boolean = false) {
		super.updateLayout(processChildren);

		if (AdaptiveCard.useAdvancedTextBlockTruncation && this.maxLines && this.isRendered()) {
			// Reset the element's innerHTML in case the available room for
			// content has increased
			this.restoreOriginalContent();
			var maxHeight = this._computedLineHeight * this.maxLines;
			this.truncateIfSupported(maxHeight);
		}
	}

	get text(): string {
		return this._text;
	}

	set text(value: string) {
		if (this._text != value) {
			this._text = value;

			this._processedText = null;
		}
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