import * as Enums from "./enums";
import * as Utils from "./utils";
import * as HostConfig from "./host-config";
// import { Image } from "./elements/image";
// import { ShowCardAction } from "./actions/show-card-action";
import { ElementTypeRegistry, ActionTypeRegistry } from "./type-registry";
// import { AdaptiveCard, InlineAdaptiveCard } from "./elements/adaptive-card";
import { Container } from "./elements/container";
// import * as TextFormatters from "./text-formatters";

export class SpacingDefinition {
	left: number = 0;
	top: number = 0;
	right: number = 0;
	bottom: number = 0;

	constructor(top: number = 0,
		right: number = 0,
		bottom: number = 0,
		left: number = 0) {
		this.top = top;
		this.right = right;
		this.bottom = bottom;
		this.left = left;
	}
}

export class PaddingDefinition {
	top: Enums.Spacing = Enums.Spacing.None;
	right: Enums.Spacing = Enums.Spacing.None;
	bottom: Enums.Spacing = Enums.Spacing.None;
	left: Enums.Spacing = Enums.Spacing.None;

	constructor(top: Enums.Spacing = Enums.Spacing.None,
		right: Enums.Spacing = Enums.Spacing.None,
		bottom: Enums.Spacing = Enums.Spacing.None,
		left: Enums.Spacing = Enums.Spacing.None) {
		this.top = top;
		this.right = right;
		this.bottom = bottom;
		this.left = left;
	}

	toSpacingDefinition(hostConfig: HostConfig.HostConfig): SpacingDefinition {
		return new SpacingDefinition(
			hostConfig.getEffectiveSpacing(this.top),
			hostConfig.getEffectiveSpacing(this.right),
			hostConfig.getEffectiveSpacing(this.bottom),
			hostConfig.getEffectiveSpacing(this.left));
	}
}

export interface IResourceInformation {
	url: string;
	mimeType: string;
}

export abstract class CardObject {
	protected static raiseParseError(error: HostConfig.IValidationError, errors: Array<HostConfig.IValidationError>) {
		if (errors) {
			errors.push(error);
		}
	
		if (CardObject.onParseError != null) {
			CardObject.onParseError(error);
		}
	}
	
	protected static raiseImageLoadedEvent(image: CardElement) {
		let rootElement = image.getRootElement();
		let onImageLoadedHandler = (rootElement && rootElement.onImageLoaded) ? rootElement.onImageLoaded : CardObject.onImageLoaded;
	
		if (onImageLoadedHandler) {
			onImageLoadedHandler(image);
		}
	}
	
	protected static raiseAnchorClickedEvent(element: CardElement, anchor: HTMLAnchorElement): boolean {
		let rootElement = element.getRootElement();
		let onAnchorClickedHandler = (rootElement && rootElement.onAnchorClicked) ? rootElement.onAnchorClicked : CardObject.onAnchorClicked;
	
		return onAnchorClickedHandler != null ? onAnchorClickedHandler(element, anchor) : false;
	}
	
	protected static raiseExecuteActionEvent(action: Action) {
		let rootElement = action.parent.getRootElement();
		let onExecuteActionHandler = (rootElement && rootElement.onExecuteAction) ? rootElement.onExecuteAction : CardObject.onExecuteAction;
	
		if (onExecuteActionHandler) {
			action.prepare(action.parent.getRootElement().getAllInputs());
	
			onExecuteActionHandler(action);
		}
	}
	
	protected static raiseElementVisibilityChangedEvent(element: CardElement, shouldUpdateLayout: boolean = true) {
		let rootElement = element.getRootElement();
	
		if (shouldUpdateLayout) {
			rootElement.updateLayout();
		}
	
		let onElementVisibilityChangedHandler = (element && element.onElementVisibilityChanged) ? element.onElementVisibilityChanged : CardObject.onElementVisibilityChanged;
	
		if (onElementVisibilityChangedHandler != null) {
			onElementVisibilityChangedHandler(element);
		}
	}
	
	protected static raiseParseElementEvent(element: CardElement, json: any, errors?: Array<HostConfig.IValidationError>) {
		let rootElement = element.getRootElement();
		let onParseElementHandler = (rootElement && rootElement.onParseElement) ? rootElement.onParseElement : CardObject.onParseElement;
	
		if (onParseElementHandler != null) {
			onParseElementHandler(element, json, errors);
		}
	}
	
	protected static raiseParseActionEvent(action: Action, json: any, errors?: Array<HostConfig.IValidationError>) {
		let rootElement = action.parent ? action.parent.getRootElement() : null;
		let onParseActionHandler = (rootElement && rootElement.onParseAction) ? rootElement.onParseAction : CardObject.onParseAction;
	
		if (onParseActionHandler != null) {
			onParseActionHandler(action, json, errors);
		}
	}
	
	protected static createCardObjectInstance<T extends CardObject>(
		parent: CardElement,
		json: any,
		createInstanceCallback: (typeName: string) => T,
		createValidationErrorCallback: (typeName: string) => HostConfig.IValidationError,
		errors: Array<HostConfig.IValidationError>): T {
		let result: T = null;
	
		if (json && typeof json === "object") {
			let tryToFallback = false;
			let typeName = json["type"];
	
			result = createInstanceCallback(typeName);
	
			if (!result) {
				tryToFallback = true;
	
				CardObject.raiseParseError(createValidationErrorCallback(typeName), errors);
			}
			else {
				result.setParent(parent);
				result.parse(json);
	
				tryToFallback = result.shouldFallback();
			}
	
			if (tryToFallback) {
				let fallback = json["fallback"];
	
				if (!fallback) {
					parent.setShouldFallback(true);
				}
				if (typeof fallback === "string" && fallback.toLowerCase() === "drop") {
					result = null;
				}
				else if (typeof fallback === "object") {
					result = CardObject.createCardObjectInstance<T>(
						parent,
						fallback,
						createInstanceCallback,
						createValidationErrorCallback,
						errors);
				}
			}
		}
	
		return result;
	}

	// This needs to be public because it is called from ActionCollection
	static raiseInlineCardExpandedEvent(action: Action, isExpanded: boolean) {
		let rootElement = action.parent.getRootElement();
		let onInlineCardExpandedHandler = (rootElement && rootElement.onInlineCardExpanded) ? rootElement.onInlineCardExpanded : CardObject.onInlineCardExpanded;
	
		if (onInlineCardExpandedHandler) {
			onInlineCardExpandedHandler(action, isExpanded);
		}
	}
	
	static onAnchorClicked: (element: CardElement, anchor: HTMLAnchorElement) => boolean = null;
	static onExecuteAction: (action: Action) => void = null;
	static onElementVisibilityChanged: (element: CardElement) => void = null;
	static onImageLoaded: (image: CardElement) => void = null;
	static onInlineCardExpanded: (action: Action, isExpanded: boolean) => void = null;
	static onParseElement: (element: CardElement, json: any, errors?: Array<HostConfig.IValidationError>) => void = null;
	static onParseAction: (element: Action, json: any, errors?: Array<HostConfig.IValidationError>) => void = null;
	static onParseError: (error: HostConfig.IValidationError) => void = null;
	static onProcessMarkdown: (text: string, result: IMarkdownProcessingResult) => void = null;

	static useAdvancedTextBlockTruncation: boolean = true;
	static useAdvancedCardBottomTruncation: boolean = false;
	static useMarkdownInRadioButtonAndCheckbox: boolean = true;
	static allowMarkForTextHighlighting: boolean = false;

	static readonly elementTypeRegistry = new ElementTypeRegistry();
	static readonly actionTypeRegistry = new ActionTypeRegistry();

	static get processMarkdown(): (text: string) => string {
		throw new Error("The processMarkdown event has been removed. Please update your code and set onProcessMarkdown instead.")
	}

	static set processMarkdown(value: (text: string) => string) {
		throw new Error("The processMarkdown event has been removed. Please update your code and set onProcessMarkdown instead.")
	}

	static applyMarkdown(text: string): IMarkdownProcessingResult {
		let result: IMarkdownProcessingResult = {
			didProcess: false
		};

		if (CardObject.onProcessMarkdown) {
			CardObject.onProcessMarkdown(text, result);
		}
		else if (window["markdownit"]) {
			// Check for markdownit
			result.outputHtml = window["markdownit"]().render(text);
			result.didProcess = true;
		} else {
			console.warn("Markdown processing isn't enabled. Please see https://www.npmjs.com/package/adaptivecards#supporting-markdown")
		}

		return result;
	}

	onAnchorClicked: (element: CardElement, anchor: HTMLAnchorElement) => boolean = null;
	onExecuteAction: (action: Action) => void = null;
	onElementVisibilityChanged: (element: CardElement) => void = null;
	onImageLoaded: (image: CardElement) => void = null;
	onInlineCardExpanded: (action: Action, isExpanded: boolean) => void = null;
	onParseElement: (element: CardElement, json: any, errors?: Array<HostConfig.IValidationError>) => void = null;
	onParseAction: (element: Action, json: any, errors?: Array<HostConfig.IValidationError>) => void = null;

	abstract shouldFallback(): boolean;
	abstract setParent(parent: CardElement);
	abstract parse(json: any);
}

export abstract class Action extends CardObject {
	static createActionInstance(
		parent: CardElement,
		json: any,
		errors: Array<HostConfig.IValidationError>): Action {
		return CardObject.createCardObjectInstance<Action>(
			parent,
			json,
			(typeName: string) => { return CardObject.actionTypeRegistry.createInstance(typeName); },
			(typeName: string) => {
				return {
					error: Enums.ValidationError.UnknownActionType,
					message: "Unknown action type: " + typeName + ". Attempting to fall back."
				}
			},
			errors);
	}
	
	private _shouldFallback: boolean = false;
	private _parent: CardElement = null;
	private _actionCollection: ActionCollection = null; // hold the reference to its action collection
	private _renderedElement: HTMLElement = null;

	protected addCssClasses(element: HTMLElement) {
		// Do nothing in base implementation
	}

	abstract getJsonTypeName(): string;

	readonly requires = new HostConfig.HostCapabilities();

	id: string;
	title: string;
	iconUrl: string;
	isPrimary: boolean;

	onExecute: (sender: Action) => void;

	expand(suppressStyle: boolean): HTMLElement {
		return null;
	}

	setCollection(actionCollection: ActionCollection) {
		this._actionCollection = actionCollection;
	}

	toJSON() {
		let result = {};

		Utils.setProperty(result, "type", this.getJsonTypeName());
		Utils.setProperty(result, "id", this.id);
		Utils.setProperty(result, "title", this.title);
		Utils.setProperty(result, "iconUrl", this.iconUrl);

		return result;
	}

	render() {
		// Cache hostConfig for perf
		let hostConfig = this.parent.hostConfig;

		var buttonElement = document.createElement("button");
		buttonElement.className = hostConfig.makeCssClassName("ac-pushButton");

		this.addCssClasses(buttonElement);

		buttonElement.setAttribute("aria-label", this.title);
		buttonElement.type = "button";
		buttonElement.style.display = "flex";
		buttonElement.style.alignItems = "center";
		buttonElement.style.justifyContent = "center";

		let hasTitle = !Utils.isNullOrEmpty(this.title);

		let titleElement = document.createElement("div");
		titleElement.style.overflow = "hidden";
		titleElement.style.textOverflow = "ellipsis";

		if (!(hostConfig.actions.iconPlacement == Enums.ActionIconPlacement.AboveTitle || hostConfig.actions.allowTitleToWrap)) {
			titleElement.style.whiteSpace = "nowrap";
		}

		if (hasTitle) {
			titleElement.innerText = this.title;
		}

		if (Utils.isNullOrEmpty(this.iconUrl)) {
			buttonElement.classList.add("noIcon");

			buttonElement.appendChild(titleElement);
		}
		else {
			let iconElement = document.createElement("img");
			iconElement.src = this.iconUrl;
			iconElement.style.width = hostConfig.actions.iconSize + "px";
			iconElement.style.height = hostConfig.actions.iconSize + "px";
			iconElement.style.flex = "0 0 auto";

			if (hostConfig.actions.iconPlacement == Enums.ActionIconPlacement.AboveTitle) {
				buttonElement.classList.add("iconAbove");
				buttonElement.style.flexDirection = "column";

				if (hasTitle) {
					iconElement.style.marginBottom = "4px";
				}
			}
			else {
				buttonElement.classList.add("iconLeft");

				if (hasTitle) {
					iconElement.style.marginRight = "4px";
				}
			}

			buttonElement.appendChild(iconElement);
			buttonElement.appendChild(titleElement);
		}

		this._renderedElement = buttonElement;
	}

	setParent(value: CardElement) {
		this._parent = value;
	}

	execute() {
		if (this.onExecute) {
			this.onExecute(this);
		}

		CardObject.raiseExecuteActionEvent(this);
	}

	// Expand the action card pane with a inline status card
	// Null status will clear the status bar
	/*
	setStatus(status: any) {
		if (this._actionCollection == null) {
			return;
		}

		if (status) {
			let statusCard = new InlineAdaptiveCard();
			statusCard.parse(status);
			this._actionCollection.showStatusCard(statusCard);
		}
		else {
			this._actionCollection.hideStatusCard();
		}
	}
	*/

	validate(): Array<HostConfig.IValidationError> {
		return [];
	}

	prepare(inputs: Array<Input>) {
		// Do nothing in base implementation
	};

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		CardObject.raiseParseActionEvent(this, json, errors);

		this.requires.parse(json["requires"], errors);
		this.id = json["id"];

		if (!json["title"] && json["title"] !== "") {
			CardObject.raiseParseError(
				{
					error: Enums.ValidationError.PropertyCantBeNull,
					message: "Actions should always have a title."
				},
				errors
			);
		}

		this.title = json["title"];
		this.iconUrl = json["iconUrl"];
	}

	remove(): boolean {
		if (this._actionCollection) {
			return this._actionCollection.removeAction(this);
		}

		return false;
	}

	getAllInputs(): Array<Input> {
		return [];
	}

	getResourceInformation(): Array<IResourceInformation> {
		if (!Utils.isNullOrEmpty(this.iconUrl)) {
			return [{ url: this.iconUrl, mimeType: "image" }]
		}
		else {
			return [];
		}
	}

	getActionById(id: string): Action {
		if (this.id == id) {
			return this;
		}
	}

	shouldFallback(): boolean {
		return this._shouldFallback || !this.requires.areAllMet(this.parent.hostConfig.hostCapabilities);
	}

	get parent(): CardElement {
		return this._parent;
	}

	get renderedElement(): HTMLElement {
		return this._renderedElement;
	}

	get isExpandable(): boolean {
		return false;
	}
}

export abstract class CardElement extends CardObject {
	static createCardElementInstance(
		parent: CardElement,
		json: any,
		errors: Array<HostConfig.IValidationError>): CardElement {
		return CardObject.createCardObjectInstance<CardElement>(
			parent,
			json,
			(typeName: string) => { return CardObject.elementTypeRegistry.createInstance(typeName); },
			(typeName: string) => {
				return {
					error: Enums.ValidationError.UnknownElementType,
					message: "Unknown element type: " + typeName + ". Attempting to fall back."
				}
			},
			errors);
	}
	
	private _shouldFallback: boolean = false;
	private _lang: string = undefined;
	private _hostConfig?: HostConfig.HostConfig = null;
	private _internalPadding: PaddingDefinition = null;
	private _parent: CardElement = null;
	private _renderedElement: HTMLElement = null;
	private _separatorElement: HTMLElement = null;
	private _isVisible: boolean = true;
	private _truncatedDueToOverflow: boolean = false;
	private _defaultRenderedElementDisplayMode: string = null;
	private _padding: PaddingDefinition = null;

	private internalRenderSeparator(): HTMLElement {
		return Utils.renderSeparation(
			{
				spacing: this.hostConfig.getEffectiveSpacing(this.spacing),
				lineThickness: this.separator ? this.hostConfig.separator.lineThickness : null,
				lineColor: this.separator ? this.hostConfig.separator.lineColor : null
			},
			this.separatorOrientation);
	}

	private updateRenderedElementVisibility() {
		if (this._renderedElement) {
			this._renderedElement.style.display = this._isVisible ? this._defaultRenderedElementDisplayMode : "none";
		}

		if (this._separatorElement) {
			if (this.parent && this.parent.isFirstElement(this)) {
				this._separatorElement.style.display = "none";
			}
			else {
				this._separatorElement.style.display = this._isVisible ? this._defaultRenderedElementDisplayMode : "none";
			}
		}
	}

	private hideElementDueToOverflow() {
		if (this._renderedElement && this._isVisible) {
			this._renderedElement.style.visibility = 'hidden';
			this._isVisible = false;
			CardObject.raiseElementVisibilityChangedEvent(this, false);
		}
	}

	private showElementHiddenDueToOverflow() {
		if (this._renderedElement && !this._isVisible) {
			this._renderedElement.style.visibility = null;
			this._isVisible = true;
			CardObject.raiseElementVisibilityChangedEvent(this, false);
		}
	}

	// Marked private to emulate internal access
	private handleOverflow(maxHeight: number) {
		if (this.isVisible || this.isHiddenDueToOverflow()) {
			var handled = this.truncateOverflow(maxHeight);

			// Even if we were unable to truncate the element to fit this time,
			// it still could have been previously truncated
			this._truncatedDueToOverflow = handled || this._truncatedDueToOverflow;

			if (!handled) {
				this.hideElementDueToOverflow();
			}
			else if (handled && !this._isVisible) {
				this.showElementHiddenDueToOverflow();
			}
		}
	}

	// Marked private to emulate internal access
	private resetOverflow(): boolean {
		var sizeChanged = false;

		if (this._truncatedDueToOverflow) {
			this.undoOverflowTruncation();
			this._truncatedDueToOverflow = false;
			sizeChanged = true;
		}

		if (this.isHiddenDueToOverflow) {
			this.showElementHiddenDueToOverflow();
		}

		return sizeChanged;
	}

	protected createPlaceholderElement(): HTMLElement {
		var element = document.createElement("div");
		element.style.border = "1px dashed #DDDDDD";
		element.style.padding = "4px";
		element.style.minHeight = "32px";
		element.style.fontSize = "10px";
		element.innerText = "Empty " + this.getJsonTypeName();

		return element;
	}

	protected internalGetNonZeroPadding(padding: PaddingDefinition,
		processTop: boolean = true,
		processRight: boolean = true,
		processBottom: boolean = true,
		processLeft: boolean = true) {
		if (processTop) {
			if (padding.top == Enums.Spacing.None) {
				padding.top = this.internalPadding.top;
			}
		}

		if (processRight) {
			if (padding.right == Enums.Spacing.None) {
				padding.right = this.internalPadding.right;
			}
		}

		if (processBottom) {
			if (padding.bottom == Enums.Spacing.None) {
				padding.bottom = this.internalPadding.bottom;
			}
		}

		if (processLeft) {
			if (padding.left == Enums.Spacing.None) {
				padding.left = this.internalPadding.left;
			}
		}

		if (this.parent) {
			this.parent.internalGetNonZeroPadding(
				padding,
				processTop && this.isAtTheVeryTop(),
				processRight && this.isAtTheVeryRight(),
				processBottom && this.isAtTheVeryBottom(),
				processLeft && this.isAtTheVeryLeft());
		}
	}

	protected adjustRenderedElementSize(renderedElement: HTMLElement) {
		if (this.height === "auto") {
			renderedElement.style.flex = "0 0 auto";
		}
		else {
			renderedElement.style.flex = "1 1 auto";
		}
	}

	protected abstract internalRender(): HTMLElement;

    /*
     * Called when this element overflows the bottom of the card.
     * maxHeight will be the amount of space still available on the card (0 if
     * the element is fully off the card).
     */
	protected truncateOverflow(maxHeight: number): boolean {
		// Child implementations should return true if the element handled
		// the truncation request such that its content fits within maxHeight,
		// false if the element should fall back to being hidden
		return false;
	}

    /*
     * This should reverse any changes performed in truncateOverflow().
     */
	protected undoOverflowTruncation() { }

	protected isDesignMode(): boolean {
		var rootElement = this.getRootElement();

		return rootElement.isDesignMode();
	}

	protected get useDefaultSizing(): boolean {
		return true;
	}

	protected get allowCustomPadding(): boolean {
		return true;
	}

	protected get defaultPadding(): PaddingDefinition {
		return new PaddingDefinition();
	}

	protected get internalPadding(): PaddingDefinition {
		if (this._padding) {
			return this._padding;
		}
		else {
			return (this._internalPadding && this.allowCustomPadding) ? this._internalPadding : this.defaultPadding;
		}
	}

	protected set internalPadding(value: PaddingDefinition) {
		this._internalPadding = value;
	}

	protected get separatorOrientation(): Enums.Orientation {
		return Enums.Orientation.Horizontal;
	}

	protected getPadding(): PaddingDefinition {
		return this._padding;
	}

	protected setPadding(value: PaddingDefinition) {
		this._padding = value;
	}

	readonly requires = new HostConfig.HostCapabilities();

	id: string;
	speak: string;
	horizontalAlignment?: Enums.HorizontalAlignment = null;
	spacing: Enums.Spacing = Enums.Spacing.Default;
	separator: boolean = false;
	height: "auto" | "stretch" = "auto";
	customCssSelector: string = null;

	abstract getJsonTypeName(): string;
	abstract renderSpeech(): string;

	toJSON() {
		let result = {};

		Utils.setProperty(result, "type", this.getJsonTypeName());
		Utils.setProperty(result, "id", this.id);

		if (this.horizontalAlignment !== null) {
			Utils.setEnumProperty(Enums.HorizontalAlignment, result, "horizontalAlignment", this.horizontalAlignment);
		}

		Utils.setEnumProperty(Enums.Spacing, result, "spacing", this.spacing, Enums.Spacing.Default);
		Utils.setProperty(result, "separator", this.separator, false);
		Utils.setProperty(result, "height", this.height, "auto");

		return result;
	}

	setParent(value: CardElement) {
		this._parent = value;
	}

	getNonZeroPadding(): PaddingDefinition {
		var padding: PaddingDefinition = new PaddingDefinition();

		this.internalGetNonZeroPadding(padding);

		return padding;
	}

	getForbiddenElementTypes(): Array<string> {
		return null;
	}

	getForbiddenActionTypes(): Array<any> {
		return null;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		CardObject.raiseParseElementEvent(this, json, errors);

		this.requires.parse(json["requires"], errors);
		this.id = json["id"];
		this.speak = json["speak"];
		this.horizontalAlignment = Utils.getEnumValueOrDefault(Enums.HorizontalAlignment, json["horizontalAlignment"], null);

		this.spacing = Utils.getEnumValueOrDefault(Enums.Spacing, json["spacing"], Enums.Spacing.Default);
		this.separator = json["separator"];

		var jsonSeparation = json["separation"];

		if (jsonSeparation !== undefined) {
			if (jsonSeparation === "none") {
				this.spacing = Enums.Spacing.None;
				this.separator = false;
			}
			else if (jsonSeparation === "strong") {
				this.spacing = Enums.Spacing.Large;
				this.separator = true;
			}
			else if (jsonSeparation === "default") {
				this.spacing = Enums.Spacing.Default;
				this.separator = false;
			}

			CardObject.raiseParseError(
				{
					error: Enums.ValidationError.Deprecated,
					message: "The \"separation\" property is deprecated and will be removed. Use the \"spacing\" and \"separator\" properties instead."
				},
				errors
			);
		}

		var jsonHeight = json["height"];

		if (jsonHeight === "auto" || jsonHeight === "stretch") {
			this.height = jsonHeight;
		}
	}

	getActionCount(): number {
		return 0;
	}

	getActionAt(index: number): Action {
		throw new Error("Index out of range.");
	}

	validate(): Array<HostConfig.IValidationError> {
		return [];
	}

	remove(): boolean {
		if (this.parent && this.parent instanceof CardElementContainer) {
			return this.parent.removeItem(this);
		}

		return false;
	}

	render(): HTMLElement {
		this._renderedElement = this.internalRender();
		this._separatorElement = this.internalRenderSeparator();

		if (this._renderedElement) {
			if (this.customCssSelector) {
				this._renderedElement.classList.add(this.customCssSelector);
			}

			this._renderedElement.style.boxSizing = "border-box";
			this._defaultRenderedElementDisplayMode = this._renderedElement.style.display;

			this.adjustRenderedElementSize(this._renderedElement);
			this.updateLayout(false);
		}
		else if (this.isDesignMode()) {
			this._renderedElement = this.createPlaceholderElement();
		}

		return this._renderedElement;
	}

	updateLayout(processChildren: boolean = true) {
		this.updateRenderedElementVisibility();
	}

	indexOf(cardElement: CardElement): number {
		return -1;
	}

	isRendered(): boolean {
		return this._renderedElement && this._renderedElement.offsetHeight > 0;
	}

	isAtTheVeryTop(): boolean {
		return this.parent ? this.parent.isFirstElement(this) && this.parent.isAtTheVeryTop() : true;
	}

	isFirstElement(element: CardElement): boolean {
		return true;
	}

	isAtTheVeryBottom(): boolean {
		return this.parent ? this.parent.isLastElement(this) && this.parent.isAtTheVeryBottom() : true;
	}

	isLastElement(element: CardElement): boolean {
		return true;
	}

	isAtTheVeryLeft(): boolean {
		return this.parent ? this.parent.isLeftMostElement(this) && this.parent.isAtTheVeryLeft() : true;
	}

	isBleeding(): boolean {
		return false;
	}

	isLeftMostElement(element: CardElement): boolean {
		return true;
	}

	isAtTheVeryRight(): boolean {
		return this.parent ? this.parent.isRightMostElement(this) && this.parent.isAtTheVeryRight() : true;
	}

	isRightMostElement(element: CardElement): boolean {
		return true;
	}

	isHiddenDueToOverflow(): boolean {
		return this._renderedElement && this._renderedElement.style.visibility == 'hidden';
	}

	getRootElement(): CardElement {
		var rootElement: CardElement = this;

		while (rootElement.parent) {
			rootElement = rootElement.parent;
		}

		return rootElement;
	}

	getParentContainer(): Container {
		var currentElement: CardElement = this.parent;

		while (currentElement) {
			if (currentElement instanceof Container) {
				return <Container>currentElement;
			}

			currentElement = currentElement.parent;
		}

		return null;
	}

	getAllInputs(): Array<Input> {
		return [];
	}

	getResourceInformation(): Array<IResourceInformation> {
		return [];
	}

	getElementById(id: string): CardElement {
		return this.id === id ? this : null;
	}

	getActionById(id: string): Action {
		return null;
	}

	shouldFallback(): boolean {
		return this._shouldFallback || !this.requires.areAllMet(this.hostConfig.hostCapabilities);
	}

	setShouldFallback(value: boolean) {
		this._shouldFallback = value;
	}

	get lang(): string {
		if (this._lang) {
			return this._lang;
		}
		else {
			if (this.parent) {
				return this.parent.lang;
			}
			else {
				return undefined;
			}
		}
	}

	set lang(value: string) {
		if (value && value != "") {
			var regEx = /^[a-z]{2,3}$/ig;

			var matches = regEx.exec(value);

			if (!matches) {
				throw new Error("Invalid language identifier: " + value);
			}
		}

		this._lang = value;
	}

	get hostConfig(): HostConfig.HostConfig {
		if (this._hostConfig) {
			return this._hostConfig;
		}
		else {
			if (this.parent) {
				return this.parent.hostConfig;
			}
			else {
				return defaultHostConfig;
			}
		}
	}

	set hostConfig(value: HostConfig.HostConfig) {
		this._hostConfig = value;
	}

	get index(): number {
		if (this.parent) {
			return this.parent.indexOf(this);
		}
		else {
			return 0;
		}
	}

	get isInteractive(): boolean {
		return false;
	}

	get isStandalone(): boolean {
		return true;
	}

	get parent(): CardElement {
		return this._parent;
	}

	get isVisible(): boolean {
		return this._isVisible;
	}

	get hasVisibleSeparator(): boolean {
		var parentContainer = this.getParentContainer();

		if (parentContainer) {
			return this.separatorElement && !parentContainer.isFirstElement(this);
		}
		else {
			return false;
		}
	}

	set isVisible(value: boolean) {
		// If the element is going to be hidden, reset any changes that were due
		// to overflow truncation (this ensures that if the element is later
		// un-hidden it has the right content)
		if (CardObject.useAdvancedCardBottomTruncation && !value) {
			this.undoOverflowTruncation();
		}

		if (this._isVisible != value) {
			this._isVisible = value;

			this.updateRenderedElementVisibility();

			if (this._renderedElement) {
				CardObject.raiseElementVisibilityChangedEvent(this);
			}
		}
	}

	get renderedElement(): HTMLElement {
		return this._renderedElement;
	}

	get separatorElement(): HTMLElement {
		return this._separatorElement;
	}
}

export abstract class CardElementContainer extends CardElement {
	abstract getItemCount(): number;
	abstract getItemAt(index: number): CardElement;
	abstract removeItem(item: CardElement): boolean;
}

export abstract class Input extends CardElement implements Utils.IInput {
	protected valueChanged() {
		if (this.onValueChanged) {
			this.onValueChanged(this);
		}
	}

	abstract get value(): string;

	onValueChanged: (sender: Input) => void;

	title: string;
	defaultValue: string;

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "title", this.title);
		Utils.setProperty(result, "value", this.renderedElement ? this.value : this.defaultValue);

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		if (!this.id) {
			return [{ error: Enums.ValidationError.PropertyCantBeNull, message: "All inputs must have a unique Id" }];
		}
		else {
			return [];
		}
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.id = json["id"];
		this.defaultValue = json["value"];
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak;
		}

		if (this.title) {
			return '<s>' + this.title + '</s>\n';
		}

		return null;
	}

	getAllInputs(): Array<Input> {
		return [this];
	}

	get isInteractive(): boolean {
		return true;
	}
}

enum ActionButtonState {
	Normal,
	Expanded,
	Subdued
}

class ActionButton {
	private _parentContainerStyle: string;
	private _action: Action;
	private _element: HTMLButtonElement = null;
	private _state: ActionButtonState = ActionButtonState.Normal;

	private updateCssStyle() {
		let hostConfig = this.action.parent.hostConfig;

		this.action.renderedElement.className = hostConfig.makeCssClassName("ac-pushButton");
		this.action.renderedElement.classList.add("style-" + this._parentContainerStyle);

		if (this.action.isExpandable) {
			this.action.renderedElement.classList.add(hostConfig.makeCssClassName("expandable"));
		}

		this.action.renderedElement.classList.remove(hostConfig.makeCssClassName("expanded"));
		this.action.renderedElement.classList.remove(hostConfig.makeCssClassName("subdued"));

		switch (this._state) {
			case ActionButtonState.Expanded:
				this.action.renderedElement.classList.add(hostConfig.makeCssClassName("expanded"));
				break;
			case ActionButtonState.Subdued:
				this.action.renderedElement.classList.add(hostConfig.makeCssClassName("subdued"));
				break;
		}

		if (this.action.isPrimary) {
			this.action.renderedElement.classList.add(hostConfig.makeCssClassName("primary"));
		}

	}

	readonly action: Action;

	constructor(action: Action, parentContainerStyle: string) {
		this.action = action;
		this._parentContainerStyle = parentContainerStyle;
	}

	onClick: (actionButton: ActionButton) => void = null;

	render(alignment: Enums.ActionAlignment) {
		this.action.render();
		this.action.renderedElement.style.flex = alignment === Enums.ActionAlignment.Stretch ? "0 1 100%" : "0 1 auto";
		this.action.renderedElement.onclick = (e) => { this.click(); };

		this.updateCssStyle();
	}

	click() {
		if (this.onClick != null) {
			this.onClick(this);
		}
	}

	get state(): ActionButtonState {
		return this._state;
	}

	set state(value: ActionButtonState) {
		this._state = value;

		this.updateCssStyle();
	}
}

export class ActionCollection {
	private static isActionAllowed(action: Action, forbiddenActionTypes: Array<string>): boolean {
		if (forbiddenActionTypes) {
			for (var i = 0; i < forbiddenActionTypes.length; i++) {
				if (action.getJsonTypeName() === forbiddenActionTypes[i]) {
					return false;
				}
			}
		}
	
		return true;
	}
	
	private _owner: CardElement;
	private _actionCardContainer: HTMLDivElement;
	private _expandedAction: Action = null;
	private _renderedActionCount: number = 0;
	private _statusCard: HTMLElement = null;
	private _actionCard: HTMLElement = null;

	private refreshContainer() {
		this._actionCardContainer.innerHTML = "";

		if (this._actionCard === null && this._statusCard === null) {
			this._actionCardContainer.style.padding = "0px";
			this._actionCardContainer.style.marginTop = "0px";

			return;
		}

		this._actionCardContainer.style.marginTop = this._renderedActionCount > 0 ? this._owner.hostConfig.actions.showCard.inlineTopMargin + "px" : "0px";

		var padding = this._owner.getNonZeroPadding().toSpacingDefinition(this._owner.hostConfig);

		if (this._actionCard !== null) {
			this._actionCard.style.paddingLeft = padding.left + "px";
			this._actionCard.style.paddingRight = padding.right + "px";

			this._actionCard.style.marginLeft = "-" + padding.left + "px";
			this._actionCard.style.marginRight = "-" + padding.right + "px";

			Utils.appendChild(this._actionCardContainer, this._actionCard);
		}

		if (this._statusCard !== null) {
			this._statusCard.style.paddingLeft = padding.left + "px";
			this._statusCard.style.paddingRight = padding.right + "px";

			this._statusCard.style.marginLeft = "-" + padding.left + "px";
			this._statusCard.style.marginRight = "-" + padding.right + "px";

			Utils.appendChild(this._actionCardContainer, this._statusCard);
		}
	}

	private layoutChanged() {
		this._owner.getRootElement().updateLayout();
	}

	private hideActionCard() {
		var previouslyExpandedAction = this._expandedAction;

		this._expandedAction = null;
		this._actionCard = null;

		this.refreshContainer();

		if (previouslyExpandedAction) {
			this.layoutChanged();

			CardObject.raiseInlineCardExpandedEvent(previouslyExpandedAction, false);
		}
	}

	private showActionCard(action: Action, suppressStyle: boolean = false, raiseEvent: boolean = true) {
		if (action.isExpandable) {
			let expandedAction = action.expand(suppressStyle);

			if (expandedAction) {
				this._actionCard = expandedAction;
				this._expandedAction = action;

				this.refreshContainer();

				if (raiseEvent) {
					this.layoutChanged();

					CardObject.raiseInlineCardExpandedEvent(action, true);
				}
			}
		}
	}

	private collapseExpandedAction() {
		for (var i = 0; i < this.buttons.length; i++) {
			this.buttons[i].state = ActionButtonState.Normal;
		}

		this.hideActionCard();
	}

	private expandAction(action: Action, raiseEvent: boolean) {
		for (var i = 0; i < this.buttons.length; i++) {
			if (this.buttons[i].action !== action) {
				this.buttons[i].state = ActionButtonState.Subdued;
			}
			else {
				this.buttons[i].state = ActionButtonState.Expanded;
			}
		}

		this.showActionCard(
			action,
			!(this._owner.isAtTheVeryLeft() && this._owner.isAtTheVeryRight()),
			raiseEvent);
	}

	private actionClicked(actionButton: ActionButton) {
		if (!(actionButton.action.isExpandable)) {
			for (var i = 0; i < this.buttons.length; i++) {
				this.buttons[i].state = ActionButtonState.Normal;
			}

			// this.hideStatusCard();
			this.hideActionCard();

			actionButton.action.execute();
		}
		else {
			// this.hideStatusCard();

			if (this._owner.hostConfig.actions.showCard.actionMode === Enums.ShowCardActionMode.Popup) {
				actionButton.action.execute();
			}
			else if (actionButton.action === this._expandedAction) {
				this.collapseExpandedAction();
			}
			else {
				this.expandAction(actionButton.action, true);
			}
		}
	}

	private getParentContainer(): Container {
		if (this._owner instanceof Container) {
			return this._owner;
		}
		else {
			return this._owner.getParentContainer();
		}
	}

	private findActionButton(action: Action): ActionButton {
		for (let actionButton of this.buttons) {
			if (actionButton.action == action) {
				return actionButton;
			}
		}

		return null;
	}

	items: Array<Action> = [];
	buttons: Array<ActionButton> = [];

	constructor(owner: CardElement) {
		this._owner = owner;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		this.clear();

		if (json && json instanceof Array) {
			for (let jsonAction of json) {
				let action = Action.createActionInstance(
					this._owner,
					jsonAction,
					errors);

				if (action) {
					this.addAction(action);
				}
			}
		}
	}

	toJSON() {
		if (this.items.length > 0) {
			let result = [];

			for (let action of this.items) {
				result.push(action.toJSON());
			}

			return result;
		}
		else {
			return null;
		}
	}

	/*
	showStatusCard(status: AdaptiveCard) {
		status.setParent(this._owner);

		this._statusCard = status.render();

		this.refreshContainer();
	}

	hideStatusCard() {
		this._statusCard = null;

		this.refreshContainer();
	}
	*/

	getActionById(id: string): Action {
		var result: Action = null;

		for (var i = 0; i < this.items.length; i++) {
			result = this.items[i].getActionById(id);

			if (result) {
				break;
			}
		}

		return result;
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];

		if (this._owner.hostConfig.actions.maxActions && this.items.length > this._owner.hostConfig.actions.maxActions) {
			result.push(
				{
					error: Enums.ValidationError.TooManyActions,
					message: "A maximum of " + this._owner.hostConfig.actions.maxActions + " actions are allowed."
				});
		}

		if (this.items.length > 0 && !this._owner.hostConfig.supportsInteractivity) {
			result.push(
				{
					error: Enums.ValidationError.InteractivityNotAllowed,
					message: "Interactivity is not allowed."
				});
		}

		for (var i = 0; i < this.items.length; i++) {
			if (!ActionCollection.isActionAllowed(this.items[i], this._owner.getForbiddenActionTypes())) {
				result.push(
					{
						error: Enums.ValidationError.ActionTypeNotAllowed,
						message: "Actions of type " + this.items[i].getJsonTypeName() + " are not allowe."
					});
			}

		}

		for (var i = 0; i < this.items.length; i++) {
			result = result.concat(this.items[i].validate());
		}

		return result;
	}

	render(orientation: Enums.Orientation, isDesignMode: boolean): HTMLElement {
		if (!this._owner.hostConfig.supportsInteractivity) {
			return null;
		}

		let element = document.createElement("div");
		let maxActions = this._owner.hostConfig.actions.maxActions ? Math.min(this._owner.hostConfig.actions.maxActions, this.items.length) : this.items.length;
		let forbiddenActionTypes = this._owner.getForbiddenActionTypes();

		this._actionCardContainer = document.createElement("div");
		this._renderedActionCount = 0;

		if (this._owner.hostConfig.actions.preExpandSingleShowCardAction && maxActions == 1 && this.items[0].isExpandable && ActionCollection.isActionAllowed(this.items[0], forbiddenActionTypes)) {
			this.showActionCard(this.items[0], true);
			this._renderedActionCount = 1;
		}
		else {
			let buttonStrip = document.createElement("div");
			buttonStrip.className = this._owner.hostConfig.makeCssClassName("ac-actionSet");
			buttonStrip.style.display = "flex";

			if (orientation == Enums.Orientation.Horizontal) {
				buttonStrip.style.flexDirection = "row";

				if (this._owner.horizontalAlignment && this._owner.hostConfig.actions.actionAlignment != Enums.ActionAlignment.Stretch) {
					switch (this._owner.horizontalAlignment) {
						case Enums.HorizontalAlignment.Center:
							buttonStrip.style.justifyContent = "center";
							break;
						case Enums.HorizontalAlignment.Right:
							buttonStrip.style.justifyContent = "flex-end";
							break;
						default:
							buttonStrip.style.justifyContent = "flex-start";
							break;
					}
				}
				else {
					switch (this._owner.hostConfig.actions.actionAlignment) {
						case Enums.ActionAlignment.Center:
							buttonStrip.style.justifyContent = "center";
							break;
						case Enums.ActionAlignment.Right:
							buttonStrip.style.justifyContent = "flex-end";
							break;
						default:
							buttonStrip.style.justifyContent = "flex-start";
							break;
					}
				}
			}
			else {
				buttonStrip.style.flexDirection = "column";

				if (this._owner.horizontalAlignment && this._owner.hostConfig.actions.actionAlignment != Enums.ActionAlignment.Stretch) {
					switch (this._owner.horizontalAlignment) {
						case Enums.HorizontalAlignment.Center:
							buttonStrip.style.alignItems = "center";
							break;
						case Enums.HorizontalAlignment.Right:
							buttonStrip.style.alignItems = "flex-end";
							break;
						default:
							buttonStrip.style.alignItems = "flex-start";
							break;
					}
				}
				else {
					switch (this._owner.hostConfig.actions.actionAlignment) {
						case Enums.ActionAlignment.Center:
							buttonStrip.style.alignItems = "center";
							break;
						case Enums.ActionAlignment.Right:
							buttonStrip.style.alignItems = "flex-end";
							break;
						case Enums.ActionAlignment.Stretch:
							buttonStrip.style.alignItems = "stretch";
							break;
						default:
							buttonStrip.style.alignItems = "flex-start";
							break;
					}
				}
			}

			let parentContainerStyle = this.getParentContainer().style;

			for (let i = 0; i < this.items.length; i++) {
				if (ActionCollection.isActionAllowed(this.items[i], forbiddenActionTypes)) {
					let actionButton: ActionButton = this.findActionButton(this.items[i]);

					if (!actionButton) {
						actionButton = new ActionButton(this.items[i], parentContainerStyle);
						actionButton.onClick = (ab) => { this.actionClicked(ab); };

						this.buttons.push(actionButton);
					}

					actionButton.render(this._owner.hostConfig.actions.actionAlignment);

					buttonStrip.appendChild(actionButton.action.renderedElement);

					this._renderedActionCount++;

					if (this._renderedActionCount >= this._owner.hostConfig.actions.maxActions || i == this.items.length - 1) {
						break;
					}
					else if (this._owner.hostConfig.actions.buttonSpacing > 0) {
						var spacer = document.createElement("div");

						if (orientation === Enums.Orientation.Horizontal) {
							spacer.style.flex = "0 0 auto";
							spacer.style.width = this._owner.hostConfig.actions.buttonSpacing + "px";
						}
						else {
							spacer.style.height = this._owner.hostConfig.actions.buttonSpacing + "px";
						}

						Utils.appendChild(buttonStrip, spacer);
					}
				}
			}

			let buttonStripContainer = document.createElement("div");
			buttonStripContainer.style.overflow = "hidden";
			buttonStripContainer.appendChild(buttonStrip);

			Utils.appendChild(element, buttonStripContainer);
		}

		Utils.appendChild(element, this._actionCardContainer);

		for (let i = 0; i < this.buttons.length; i++) {
			if (this.buttons[i].state == ActionButtonState.Expanded) {
				this.expandAction(this.buttons[i].action, false);

				break;
			}
		}

		return this._renderedActionCount > 0 ? element : null;
	}

	addAction(action: Action) {
		if (!action) {
			throw new Error("The action parameter cannot be null.");
		}

		if ((!action.parent || action.parent === this._owner) && this.items.indexOf(action) < 0) {
			this.items.push(action);

			if (!action.parent) {
				action.setParent(this._owner);
			}

			action.setCollection(this);
		}
		else {
			throw new Error("The action already belongs to another element.");
		}
	}

	removeAction(action: Action): boolean {
		if (this.expandedAction && this._expandedAction == action) {
			this.collapseExpandedAction();
		}

		var actionIndex = this.items.indexOf(action);

		if (actionIndex >= 0) {
			this.items.splice(actionIndex, 1);

			action.setParent(null);

			action.setCollection(this);

			for (let i = 0; i < this.buttons.length; i++) {
				if (this.buttons[i].action == action) {
					this.buttons.splice(i, 1);

					break;
				}
			}

			return true;
		}

		return false;
	}

	clear() {
		this.items = [];
		this.buttons = [];

		this._expandedAction = null;
		this._renderedActionCount = 0;
	}

	getAllInputs(): Array<Input> {
		var result: Array<Input> = [];

		for (var i = 0; i < this.items.length; i++) {
			var action = this.items[i];

			result = result.concat(action.getAllInputs());
		}

		return result;
	}

	getResourceInformation(): Array<IResourceInformation> {
		let result: Array<IResourceInformation> = [];

		for (var i = 0; i < this.items.length; i++) {
			result = result.concat(this.items[i].getResourceInformation());
		}

		return result;
	}

	get renderedActionCount(): number {
		return this._renderedActionCount;
	}

	get expandedAction(): Action {
		return this._expandedAction;
	}
}

export interface IMarkdownProcessingResult {
	didProcess: boolean;
	outputHtml?: any;
}

const defaultHostConfig: HostConfig.HostConfig = new HostConfig.HostConfig(
	{
		supportsInteractivity: true,
		fontFamily: "Segoe UI",
		spacing: {
			small: 10,
			default: 20,
			medium: 30,
			large: 40,
			extraLarge: 50,
			padding: 20
		},
		separator: {
			lineThickness: 1,
			lineColor: "#EEEEEE"
		},
		fontSizes: {
			small: 12,
			default: 14,
			medium: 17,
			large: 21,
			extraLarge: 26
		},
		fontWeights: {
			lighter: 200,
			default: 400,
			bolder: 600
		},
		imageSizes: {
			small: 40,
			medium: 80,
			large: 160
		},
		containerStyles: {
			default: {
				backgroundColor: "#FFFFFF",
				foregroundColors: {
					default: {
						default: "#333333",
						subtle: "#EE333333"
					},
					dark: {
						default: "#000000",
						subtle: "#66000000"
					},
					light: {
						default: "#FFFFFF",
						subtle: "#33000000"
					},
					accent: {
						default: "#2E89FC",
						subtle: "#882E89FC"
					},
					attention: {
						default: "#cc3300",
						subtle: "#DDcc3300"
					},
					good: {
						default: "#54a254",
						subtle: "#DD54a254"
					},
					warning: {
						default: "#e69500",
						subtle: "#DDe69500"
					}
				}
			},
			emphasis: {
				backgroundColor: "#08000000",
				foregroundColors: {
					default: {
						default: "#333333",
						subtle: "#EE333333"
					},
					dark: {
						default: "#000000",
						subtle: "#66000000"
					},
					light: {
						default: "#FFFFFF",
						subtle: "#33000000"
					},
					accent: {
						default: "#2E89FC",
						subtle: "#882E89FC"
					},
					attention: {
						default: "#cc3300",
						subtle: "#DDcc3300"
					},
					good: {
						default: "#54a254",
						subtle: "#DD54a254"
					},
					warning: {
						default: "#e69500",
						subtle: "#DDe69500"
					}
				}
			}
		},
		actions: {
			maxActions: 5,
			spacing: Enums.Spacing.Default,
			buttonSpacing: 10,
			showCard: {
				actionMode: Enums.ShowCardActionMode.Inline,
				inlineTopMargin: 16
			},
			actionsOrientation: Enums.Orientation.Horizontal,
			actionAlignment: Enums.ActionAlignment.Left
		},
		adaptiveCard: {
			allowCustomStyle: false
		},
		imageSet: {
			imageSize: Enums.Size.Medium,
			maxImageHeight: 100
		},
		factSet: {
			title: {
				color: Enums.TextColor.Default,
				size: Enums.TextSize.Default,
				isSubtle: false,
				weight: Enums.TextWeight.Bolder,
				wrap: true,
				maxWidth: 150,
			},
			value: {
				color: Enums.TextColor.Default,
				size: Enums.TextSize.Default,
				isSubtle: false,
				weight: Enums.TextWeight.Default,
				wrap: true,
			},
			spacing: 10
		}
	});
