import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";
import * as Enums from "../enums";
// import { AdaptiveCard } from "../elements/adaptive-card";

export class BackgroundImage {
	url: string;
	mode: Enums.BackgroundImageMode = Enums.BackgroundImageMode.Stretch;
	horizontalAlignment: Enums.HorizontalAlignment = Enums.HorizontalAlignment.Left;
	verticalAlignment: Enums.VerticalAlignment = Enums.VerticalAlignment.Top;

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		this.url = json["url"];
		this.mode = Utils.getEnumValueOrDefault(Enums.BackgroundImageMode, json["mode"], this.mode);
		this.horizontalAlignment = Utils.getEnumValueOrDefault(Enums.HorizontalAlignment, json["horizontalAlignment"], this.horizontalAlignment);
		this.verticalAlignment = Utils.getEnumValueOrDefault(Enums.VerticalAlignment, json["verticalAlignment"], this.verticalAlignment);
	}

	apply(element: HTMLElement) {
		if (this.url) {
			element.style.backgroundImage = "url('" + this.url + "')";

			switch (this.mode) {
				case Enums.BackgroundImageMode.Repeat:
					element.style.backgroundRepeat = "repeat";
					break;
				case Enums.BackgroundImageMode.RepeatHorizontally:
					element.style.backgroundRepeat = "repeat-x";
					break;
				case Enums.BackgroundImageMode.RepeatVertically:
					element.style.backgroundRepeat = "repeat-y";
					break;
				case Enums.BackgroundImageMode.Stretch:
				default:
					element.style.backgroundRepeat = "no-repeat";
					element.style.backgroundSize = "cover";
					break;
			}

			switch (this.horizontalAlignment) {
				case Enums.HorizontalAlignment.Center:
					element.style.backgroundPositionX = "center";
					break;
				case Enums.HorizontalAlignment.Right:
					element.style.backgroundPositionX = "right";
					break;
			}

			switch (this.verticalAlignment) {
				case Enums.VerticalAlignment.Center:
					element.style.backgroundPositionY = "center";
					break;
				case Enums.VerticalAlignment.Bottom:
					element.style.backgroundPositionY = "bottom";
					break;
			}
		}
	}
}

export class Container extends Core.CardElementContainer {
	private _selectAction: Core.Action;
	private _items: Array<Core.CardElement> = [];
	private _renderedItems: Array<Core.CardElement> = [];
	private _style?: string = null;

	private isElementAllowed(element: Core.CardElement, forbiddenElementTypes: Array<string>) {
		if (!this.hostConfig.supportsInteractivity && element.isInteractive) {
			return false;
		}

		if (forbiddenElementTypes) {
			for (var i = 0; i < forbiddenElementTypes.length; i++) {
				if (element.getJsonTypeName() === forbiddenElementTypes[i]) {
					return false;
				}
			}
		}

		return true;
	}

	private insertItemAt(
		item: Core.CardElement,
		index: number,
		forceInsert: boolean) {
		if (!item.parent || forceInsert) {
			if (item.isStandalone) {
				if (index < 0 || index >= this._items.length) {
					this._items.push(item);
				}
				else {
					this._items.splice(index, 0, item);
				}

				item.setParent(this);
			}
			else {
				throw new Error("Elements of type " + item.getJsonTypeName() + " cannot be used as standalone elements.");
			}
		}
		else {
			throw new Error("The element already belongs to another container.")
		}
	}

	private get hasExplicitStyle(): boolean {
		return this._style != null;
	}

	protected getItemsCollectionPropertyName(): string {
		return "items";
	}

	protected isLastElementBleeding(): boolean {
		return this._renderedItems.length > 0 ? this._renderedItems[this._renderedItems.length - 1].isBleeding() : false;
	}

	protected applyPadding() {
		if (!this.renderedElement) {
			return;
		}

		if (this.padding) {
			var physicalPadding = this.padding.toSpacingDefinition(this.hostConfig);

			this.renderedElement.style.paddingTop = physicalPadding.top + "px";
			this.renderedElement.style.paddingRight = physicalPadding.right + "px";
			this.renderedElement.style.paddingBottom = physicalPadding.bottom + "px";
			this.renderedElement.style.paddingLeft = physicalPadding.left + "px";
		}
		else if (this.hasBackground) {
			var physicalMargin: Core.SpacingDefinition = new Core.SpacingDefinition();
			var physicalPadding: Core.SpacingDefinition = new Core.SpacingDefinition();

			physicalPadding = new Core.PaddingDefinition(
				Enums.Spacing.Padding,
				Enums.Spacing.Padding,
				Enums.Spacing.Padding,
				Enums.Spacing.Padding
			).toSpacingDefinition(this.hostConfig);

			this.renderedElement.style.marginTop = "-" + physicalMargin.top + "px";
			this.renderedElement.style.marginRight = "-" + physicalMargin.right + "px";
			this.renderedElement.style.marginBottom = "-" + physicalMargin.bottom + "px";
			this.renderedElement.style.marginLeft = "-" + physicalMargin.left + "px";

			this.renderedElement.style.paddingTop = physicalPadding.top + "px";
			this.renderedElement.style.paddingRight = physicalPadding.right + "px";
			this.renderedElement.style.paddingBottom = physicalPadding.bottom + "px";
			this.renderedElement.style.paddingLeft = physicalPadding.left + "px";

			if (this.separatorElement) {
				if (this.separatorOrientation == Enums.Orientation.Horizontal) {
					this.separatorElement.style.marginLeft = "-" + physicalMargin.left + "px";
					this.separatorElement.style.marginRight = "-" + physicalMargin.right + "px";
				}
				else {
					this.separatorElement.style.marginTop = "-" + physicalMargin.top + "px";
					this.separatorElement.style.marginBottom = "-" + physicalMargin.bottom + "px";
				}
			}
		}

		if (this.isLastElementBleeding()) {
			this.renderedElement.style.paddingBottom = "0px";
		}
	}

	protected internalRender(): HTMLElement {
		this._renderedItems = [];

		// Cache hostConfig to avoid walking the parent hierarchy several times
		let hostConfig = this.hostConfig;

		var element = document.createElement("div");

		if (this.rtl != null && this.rtl) {
			element.dir = "rtl";
		}

		element.classList.add(hostConfig.makeCssClassName("ac-container"));
		element.style.display = "flex";
		element.style.flexDirection = "column";

		if (Core.CardObject.useAdvancedCardBottomTruncation) {
			// Forces the container to be at least as tall as its content.
			//
			// Fixes a quirk in Chrome where, for nested flex elements, the
			// inner element's height would never exceed the outer element's
			// height. This caused overflow truncation to break -- containers
			// would always be measured as not overflowing, since their heights
			// were constrained by their parents as opposed to truly reflecting
			// the height of their content.
			//
			// See the "Browser Rendering Notes" section of this answer:
			// https://stackoverflow.com/questions/36247140/why-doesnt-flex-item-shrink-past-content-size
			element.style.minHeight = '-webkit-min-content';
		}

		switch (this.verticalContentAlignment) {
			case Enums.VerticalAlignment.Center:
				element.style.justifyContent = "center";
				break;
			case Enums.VerticalAlignment.Bottom:
				element.style.justifyContent = "flex-end";
				break;
			default:
				element.style.justifyContent = "flex-start";
				break;
		}

		if (this.hasBackground) {
			if (this.backgroundImage) {
				this.backgroundImage.apply(element);
			}

			let styleDefinition = this.hostConfig.containerStyles.getStyleByName(this.style, this.hostConfig.containerStyles.getStyleByName(this.defaultStyle));

			if (!Utils.isNullOrEmpty(styleDefinition.backgroundColor)) {
				element.style.backgroundColor = Utils.stringToCssColor(styleDefinition.backgroundColor);
			}
		}

		if (this.selectAction && hostConfig.supportsInteractivity) {
			element.classList.add(hostConfig.makeCssClassName("ac-selectable"));
			element.tabIndex = 0;
			element.setAttribute("role", "button");
			element.setAttribute("aria-label", this.selectAction.title);

			element.onclick = (e) => {
				if (this.selectAction != null) {
					this.selectAction.execute();
					e.cancelBubble = true;
				}
			}

			element.onkeypress = (e) => {
				if (this.selectAction != null) {
					// Enter or space pressed
					if (e.keyCode == 13 || e.keyCode == 32) {
						this.selectAction.execute();
					}
				}
			}
		}

		if (this._items.length > 0) {
			for (var i = 0; i < this._items.length; i++) {
				var renderedElement = this.isElementAllowed(this._items[i], this.getForbiddenElementTypes()) ? this._items[i].render() : null;

				if (renderedElement) {
					if (this._renderedItems.length > 0 && this._items[i].separatorElement) {
						this._items[i].separatorElement.style.flex = "0 0 auto";

						Utils.appendChild(element, this._items[i].separatorElement);
					}

					Utils.appendChild(element, renderedElement);

					this._renderedItems.push(this._items[i]);
				}
			}
		}
		else {
			if (this.isDesignMode()) {
				var placeholderElement = this.createPlaceholderElement();
				placeholderElement.style.width = "100%";
				placeholderElement.style.height = "100%";

				element.appendChild(placeholderElement);
			}
		}

		return element;
	}

	protected truncateOverflow(maxHeight: number): boolean {
		// Add 1 to account for rounding differences between browsers
		var boundary = this.renderedElement.offsetTop + maxHeight + 1;

		var handleElement = (cardElement: Core.CardElement) => {
			let elt = cardElement.renderedElement;

			if (elt) {
				switch (Utils.getFitStatus(elt, boundary)) {
					case Enums.ContainerFitStatus.FullyInContainer:
						let sizeChanged = cardElement['resetOverflow']();
						// If the element's size changed after resetting content,
						// we have to check if it still fits fully in the card
						if (sizeChanged) {
							handleElement(cardElement);
						}
						break;
					case Enums.ContainerFitStatus.Overflowing:
						let maxHeight = boundary - elt.offsetTop;
						cardElement['handleOverflow'](maxHeight);
						break;
					case Enums.ContainerFitStatus.FullyOutOfContainer:
						cardElement['handleOverflow'](0);
						break;
				}
			}
		};

		for (let item of this._items) {
			handleElement(item);
		}

		return true;
	}

	protected undoOverflowTruncation() {
		for (let item of this._items) {
			item['resetOverflow']();
		}
	}

	protected get hasBackground(): boolean {
		var parentContainer = this.getParentContainer();

		return this.backgroundImage != undefined || (this.hasExplicitStyle && (parentContainer ? parentContainer.style != this.style : false));
	}

	protected get defaultStyle(): string {
		return Enums.ContainerStyle.Default;
	}

	protected get allowCustomStyle(): boolean {
		return true;
	}

	backgroundImage: BackgroundImage;
	verticalContentAlignment: Enums.VerticalAlignment = Enums.VerticalAlignment.Top;
	rtl?: boolean = null;

	toJSON() {
		let result = super.toJSON();

		if (this._selectAction) {
			Utils.setProperty(result, "selectAction", this._selectAction.toJSON());
		}

		if (this.backgroundImage) {
			Utils.setProperty(result, "backgroundImage", this.backgroundImage.url);
		}

		Utils.setProperty(result, "style", this.style, "default");
		Utils.setEnumProperty(Enums.VerticalAlignment, result, "verticalContentAlignment", this.verticalContentAlignment, Enums.VerticalAlignment.Top);

		if (this._items.length > 0) {
			let elements = [];

			for (let element of this._items) {
				elements.push(element.toJSON());
			}

			Utils.setProperty(result, this.getItemsCollectionPropertyName(), elements);
		}

		return result;
	}

	getItemCount(): number {
		return this._items.length;
	}

	getItemAt(index: number): Core.CardElement {
		return this._items[index];
	}

	getJsonTypeName(): string {
		return "Container";
	}

	isBleeding(): boolean {
		return this.isLastElementBleeding();
	}

	isFirstElement(element: Core.CardElement): boolean {
		for (var i = 0; i < this._items.length; i++) {
			if (this._items[i].isVisible) {
				return this._items[i] == element;
			}
		}

		return false;
	}

	isLastElement(element: Core.CardElement): boolean {
		for (var i = this._items.length - 1; i >= 0; i--) {
			if (this._items[i].isVisible) {
				return this._items[i] == element;
			}
		}

		return false;
	}

	isRtl(): boolean {
		if (this.rtl != null) {
			return this.rtl;
		}
		else {
			let parentContainer = this.getParentContainer();

			return parentContainer ? parentContainer.isRtl() : false;
		}
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];

		if (this._style) {
			var styleDefinition = this.hostConfig.containerStyles.getStyleByName(this._style);

			if (!styleDefinition) {
				result.push(
					{
						error: Enums.ValidationError.InvalidPropertyValue,
						message: "Unknown container style: " + this._style
					});
			}
		}

		for (var i = 0; i < this._items.length; i++) {
			if (!this.hostConfig.supportsInteractivity && this._items[i].isInteractive) {
				result.push(
					{
						error: Enums.ValidationError.InteractivityNotAllowed,
						message: "Interactivity is not allowed."
					});
			}

			if (!this.isElementAllowed(this._items[i], this.getForbiddenElementTypes())) {
				result.push(
					{
						error: Enums.ValidationError.InteractivityNotAllowed,
						message: "Elements of type " + this._items[i].getJsonTypeName() + " are not allowed in this container."
					});
			}

			result = result.concat(this._items[i].validate());
		}

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.setShouldFallback(false);

		this._items = [];
		this._renderedItems = [];

		let jsonBackgroundImage = json["backgroundImage"];

		if (jsonBackgroundImage) {
			this.backgroundImage = new BackgroundImage();

			if (typeof jsonBackgroundImage === "string") {
				this.backgroundImage.url = jsonBackgroundImage;
				this.backgroundImage.mode = Enums.BackgroundImageMode.Stretch;
			}
			else if (typeof jsonBackgroundImage === "object") {
				this.backgroundImage = new BackgroundImage();
				this.backgroundImage.parse(json["backgroundImage"], errors);
			}
		}

		this.verticalContentAlignment = Utils.getEnumValueOrDefault(Enums.VerticalAlignment, json["verticalContentAlignment"], this.verticalContentAlignment);

		this._style = json["style"];

		this.selectAction = Core.Action.createActionInstance(
			this,
			json["selectAction"],
			errors);

		if (json[this.getItemsCollectionPropertyName()] != null) {
			let items = json[this.getItemsCollectionPropertyName()] as Array<any>;

			this.clear();

			for (let i = 0; i < items.length; i++) {
				let element = Core.CardElement.createCardElementInstance(this, items[i], errors);

				if (element) {
					this.insertItemAt(element, -1, true);
				}
			}
		}
	}

	indexOf(cardElement: Core.CardElement): number {
		return this._items.indexOf(cardElement);
	}

	addItem(item: Core.CardElement) {
		this.insertItemAt(item, -1, false);
	}

	insertItemBefore(item: Core.CardElement, insertBefore: Core.CardElement) {
		this.insertItemAt(item, this._items.indexOf(insertBefore), false);
	}

	insertItemAfter(item: Core.CardElement, insertAfter: Core.CardElement) {
		this.insertItemAt(item, this._items.indexOf(insertAfter) + 1, false);
	}

	removeItem(item: Core.CardElement): boolean {
		var itemIndex = this._items.indexOf(item);

		if (itemIndex >= 0) {
			this._items.splice(itemIndex, 1);

			item.setParent(null);

			this.updateLayout();

			return true;
		}

		return false;
	}

	clear() {
		this._items = [];
	}

	getAllInputs(): Array<Core.Input> {
		var result: Array<Core.Input> = [];

		for (var i = 0; i < this._items.length; i++) {
			var item: Core.CardElement = this._items[i];

			result = result.concat(item.getAllInputs());
		}

		return result;
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		let result: Array<Core.IResourceInformation> = [];

		if (this.backgroundImage && !Utils.isNullOrEmpty(this.backgroundImage.url)) {
			result.push({ url: this.backgroundImage.url, mimeType: "image" });
		}

		for (var i = 0; i < this.getItemCount(); i++) {
			result = result.concat(this.getItemAt(i).getResourceInformation());
		}

		return result;
	}

	getElementById(id: string): Core.CardElement {
		var result: Core.CardElement = super.getElementById(id);

		if (!result) {
			for (var i = 0; i < this._items.length; i++) {
				result = this._items[i].getElementById(id);

				if (result) {
					break;
				}
			}
		}

		return result;
	}

	getActionById(id: string): Core.Action {
		var result: Core.Action = super.getActionById(id);

		if (!result) {
			if (this.selectAction) {
				result = this.selectAction.getActionById(id);
			}

			if (!result) {
				for (var i = 0; i < this._items.length; i++) {
					result = this._items[i].getActionById(id);

					if (result) {
						break;
					}
				}
			}
		}

		return result;
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak;
		}

		// render each item
		let speak = null;

		if (this._items.length > 0) {
			speak = '';

			for (var i = 0; i < this._items.length; i++) {
				var result = this._items[i].renderSpeech();

				if (result) {
					speak += result;
				}
			}
		}

		return speak;
	}

	updateLayout(processChildren: boolean = true) {
		super.updateLayout(processChildren);

		this.applyPadding();

		if (processChildren) {
			for (var i = 0; i < this._items.length; i++) {
				this._items[i].updateLayout();
			}
		}
	}

	get style(): string {
		if (this.allowCustomStyle) {
			if (this._style && this.hostConfig.containerStyles.getStyleByName(this._style)) {
				return this._style;
			}

			return null;
		}
		else {
			return this.defaultStyle;
		}
	}

	set style(value: string) {
		this._style = value;
	}

	get padding(): Core.PaddingDefinition {
		return this.getPadding();
	}

	set padding(value: Core.PaddingDefinition) {
		this.setPadding(value);
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

export abstract class ContainerWithActions extends Container {
	private _actionCollection: Core.ActionCollection;

	protected get renderIfEmpty(): boolean {
		return false;
	}

	protected internalRender(): HTMLElement {
		var element = super.internalRender();

		var renderedActions = this._actionCollection.render(this.hostConfig.actions.actionsOrientation, false);

		if (renderedActions) {
			Utils.appendChild(
				element,
				Utils.renderSeparation(
					{
						spacing: this.hostConfig.getEffectiveSpacing(this.hostConfig.actions.spacing),
						lineThickness: null,
						lineColor: null
					},
					Enums.Orientation.Horizontal));
			Utils.appendChild(element, renderedActions);
		}

		if (this.renderIfEmpty) {
			return element;
		}
		else {
			return element.children.length > 0 ? element : null;
		}
	}

	protected isLastElementBleeding(): boolean {
		if (this._actionCollection.renderedActionCount == 0) {
			return super.isLastElementBleeding() ? !this.isDesignMode() : false;
		}
		else {
			if (this._actionCollection.items.length == 1) {
				return this._actionCollection.expandedAction != null && !this.hostConfig.actions.preExpandSingleShowCardAction;
			}
			else {
				return this._actionCollection.expandedAction != null;
			}
		}
	}

	constructor() {
		super();

		this._actionCollection = new Core.ActionCollection(this);
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "actions", this._actionCollection.toJSON());

		return result;
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

	getActionById(id: string): Core.Action {
		var result: Core.Action = this._actionCollection.getActionById(id);

		return result ? result : super.getActionById(id);
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this._actionCollection.parse(json["actions"]);
	}

	validate(): Array<HostConfig.IValidationError> {
		var result = super.validate();

		if (this._actionCollection) {
			result = result.concat(this._actionCollection.validate());
		}

		return result;
	}

	isLastElement(element: Core.CardElement): boolean {
		return super.isLastElement(element) && this._actionCollection.items.length == 0;
	}

	addAction(action: Core.Action) {
		this._actionCollection.addAction(action);
	}

	clear() {
		super.clear();

		this._actionCollection.clear();
	}

	getAllInputs(): Array<Core.Input> {
		return super.getAllInputs().concat(this._actionCollection.getAllInputs());
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		return super.getResourceInformation().concat(this._actionCollection.getResourceInformation());
	}

	get isStandalone(): boolean {
		return false;
	}
}