import * as Core from "./card-elements";
import { Utils, SizeAndUnit } from "./utils";
import * as Enums from "./enums";
import * as HostConfig from "./host-config";

export type ColumnWidth = SizeAndUnit | "auto" | "stretch";

export class Column extends Core.Container {
	private _computedWeight: number = 0;

	protected adjustRenderedElementSize(renderedElement: HTMLElement) {
		if (this.isDesignMode()) {
			renderedElement.style.minWidth = "20px";
			renderedElement.style.minHeight = "20px";
		}
		else {
			renderedElement.style.minWidth = "0";
		}

		if (this.width === "auto") {
			renderedElement.style.flex = "0 1 auto";
		}
		else if (this.width === "stretch") {
			renderedElement.style.flex = "1 1 50px";
		}
		else {
			let sizeAndUnit = <SizeAndUnit>this.width;

			if (sizeAndUnit.unit == Enums.SizeUnit.Pixel) {
				renderedElement.style.flex = "0 0 auto";
				renderedElement.style.width = sizeAndUnit.physicalSize + "px";
			}
			else {
				renderedElement.style.flex = "1 1 " + (this._computedWeight > 0 ? this._computedWeight : sizeAndUnit.physicalSize) + "%";
			}
		}
	}

	protected get separatorOrientation(): Enums.Orientation {
		return Enums.Orientation.Vertical;
	}

	width: ColumnWidth = "auto";

	constructor(width: ColumnWidth = "auto") {
		super();

		this.width = width;
	}

	getJsonTypeName(): string {
		return "Column";
	}

	toJSON() {
		let result = super.toJSON();

		if (this.width instanceof SizeAndUnit) {
			if (this.width.unit == Enums.SizeUnit.Pixel) {
				Utils.setProperty(result, "width", this.width.physicalSize + "px");
			}
			else {
				Utils.setProperty(result, "width", this.width.physicalSize);
			}
		}
		else {
			Utils.setProperty(result, "width", this.width);
		}

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		var jsonWidth = json["width"];

		if (jsonWidth === undefined) {
			jsonWidth = json["size"];

			if (jsonWidth !== undefined) {
				Core.CardObject.raiseParseError(
					{
						error: Enums.ValidationError.Deprecated,
						message: "The \"Column.size\" property is deprecated and will be removed. Use the \"Column.width\" property instead."
					},
					errors
				);
			}
		}

		var invalidWidth = false;

		try {
			this.width = SizeAndUnit.parse(jsonWidth);
		}
		catch (e) {
			if (typeof jsonWidth === "string" && (jsonWidth === "auto" || jsonWidth === "stretch")) {
				this.width = jsonWidth;
			}
			else {
				invalidWidth = true;
			}
		}

		if (invalidWidth) {
			Core.CardObject.raiseParseError(
				{
					error: Enums.ValidationError.InvalidPropertyValue,
					message: "Invalid column width:" + jsonWidth + " - defaulting to \"auto\""
				},
				errors
			);
		}
	}

	get hasVisibleSeparator(): boolean {
		if (this.parent && this.parent instanceof ColumnSet) {
			return this.separatorElement && !this.parent.isLeftMostElement(this);
		}
		else {
			return false;
		}
	}

	get isStandalone(): boolean {
		return false;
	}
}

export class ColumnSet extends Core.CardElementContainer {
	private _columns: Array<Column> = [];
	private _selectAction: Core.Action;

	protected applyPadding() {
		if (this.padding) {
			if (this.renderedElement) {
				var physicalPadding = this.padding.toSpacingDefinition(this.hostConfig);

				this.renderedElement.style.paddingTop = physicalPadding.top + "px";
				this.renderedElement.style.paddingRight = physicalPadding.right + "px";
				this.renderedElement.style.paddingBottom = physicalPadding.bottom + "px";
				this.renderedElement.style.paddingLeft = physicalPadding.left + "px";
			}
		}
	}

	protected internalRender(): HTMLElement {
		if (this._columns.length > 0) {
			// Cache hostConfig to avoid walking the parent hierarchy several times
			let hostConfig = this.hostConfig;

			var element = document.createElement("div");
			element.className = hostConfig.makeCssClassName("ac-columnSet");
			element.style.display = "flex";

			if (Core.AdaptiveCard.useAdvancedCardBottomTruncation) {
				// See comment in Container.internalRender()
				element.style.minHeight = '-webkit-min-content';
			}

			if (this.selectAction && hostConfig.supportsInteractivity) {
				element.classList.add(hostConfig.makeCssClassName("ac-selectable"));

				element.onclick = (e) => {
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

			var totalWeight: number = 0;

			for (let column of this._columns) {
				if (column.width instanceof SizeAndUnit && (column.width.unit == Enums.SizeUnit.Weight)) {
					totalWeight += column.width.physicalSize;
				}
			}

			var renderedColumnCount: number = 0;

			for (let column of this._columns) {
				if (column.width instanceof SizeAndUnit && column.width.unit == Enums.SizeUnit.Weight && totalWeight > 0) {
					var computedWeight = 100 / totalWeight * column.width.physicalSize;

					// Best way to emulate "internal" access I know of
					column["_computedWeight"] = computedWeight;
				}

				var renderedColumn = column.render();

				if (renderedColumn) {
					if (renderedColumnCount > 0 && column.separatorElement) {
						column.separatorElement.style.flex = "0 0 auto";

						Utils.appendChild(element, column.separatorElement);
					}

					Utils.appendChild(element, renderedColumn);

					renderedColumnCount++;
				}
			}

			return renderedColumnCount > 0 ? element : null;
		}
		else {
			return null;
		}
	}

	protected truncateOverflow(maxHeight: number): boolean {
		for (let column of this._columns) {
			column['handleOverflow'](maxHeight);
		}

		return true;
	}

	protected undoOverflowTruncation() {
		for (let column of this._columns) {
			column['resetOverflow']();
		}
	}

	toJSON() {
		let result = super.toJSON();

		if (this._selectAction) {
			Utils.setProperty(result, "selectAction", this.selectAction.toJSON());
		}

		if (this._columns.length > 0) {
			let columns = [];

			for (let column of this._columns) {
				columns.push(column.toJSON());
			}

			Utils.setProperty(result, "columns", columns);
		}

		return result;
	}

	isFirstElement(element: Core.CardElement): boolean {
		for (var i = 0; i < this._columns.length; i++) {
			if (this._columns[i].isVisible) {
				return this._columns[i] == element;
			}
		}

		return false;
	}

	getCount(): number {
		return this._columns.length;
	}

	getItemCount(): number {
		return this.getCount();
	}

	getColumnAt(index: number): Column {
		return this._columns[index];
	}

	getItemAt(index: number): Core.CardElement {
		return this.getColumnAt(index);
	}

	getJsonTypeName(): string {
		return "ColumnSet";
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.selectAction = Core.Action.createActionInstance(
			this,
			json["selectAction"],
			errors);

		if (json["columns"] != null) {
			let jsonColumns = json["columns"] as Array<any>;

			this._columns = [];

			for (let i = 0; i < jsonColumns.length; i++) {
				let column = new Column();
				column.setParent(this);
				column.parse(jsonColumns[i], errors);

				this._columns.push(column);
			}
		}
	}

	validate(): Array<HostConfig.IValidationError> {
		var result: Array<HostConfig.IValidationError> = [];
		var weightedColumns: number = 0;
		var stretchedColumns: number = 0;

		for (var i = 0; i < this._columns.length; i++) {
			if (typeof this._columns[i].width === "number") {
				weightedColumns++;
			}
			else if (this._columns[i].width === "stretch") {
				stretchedColumns++;
			}

			result = result.concat(this._columns[i].validate());
		}

		if (weightedColumns > 0 && stretchedColumns > 0) {
			result.push(
				{
					error: Enums.ValidationError.Hint,
					message: "It is not recommended to use weighted and stretched columns in the same ColumnSet, because in such a situation stretched columns will always get the minimum amount of space."
				});
		}

		return result;
	}

	updateLayout(processChildren: boolean = true) {
		super.updateLayout(processChildren);

		this.applyPadding();

		if (processChildren) {
			for (var i = 0; i < this._columns.length; i++) {
				this._columns[i].updateLayout();
			}
		}
	}

	addColumn(column: Column) {
		if (!column.parent) {
			this._columns.push(column);

			column.setParent(this);
		}
		else {
			throw new Error("This column already belongs to another ColumnSet.");
		}
	}

	removeItem(item: Core.CardElement): boolean {
		if (item instanceof Column) {
			var itemIndex = this._columns.indexOf(item);

			if (itemIndex >= 0) {
				this._columns.splice(itemIndex, 1);

				item.setParent(null);

				this.updateLayout();

				return true;
			}
		}

		return false;
	}

	indexOf(cardElement: Core.CardElement): number {
		return cardElement instanceof Column ? this._columns.indexOf(cardElement) : -1;
	}

	isLeftMostElement(element: Core.CardElement): boolean {
		return this._columns.indexOf(<Column>element) == 0;
	}

	isRightMostElement(element: Core.CardElement): boolean {
		return this._columns.indexOf(<Column>element) == this._columns.length - 1;
	}

	getAllInputs(): Array<Core.Input> {
		var result: Array<Core.Input> = [];

		for (var i = 0; i < this._columns.length; i++) {
			result = result.concat(this._columns[i].getAllInputs());
		}

		return result;
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		let result: Array<Core.IResourceInformation> = [];

		for (var i = 0; i < this._columns.length; i++) {
			result = result.concat(this._columns[i].getResourceInformation());
		}

		return result;
	}

	getElementById(id: string): Core.CardElement {
		var result: Core.CardElement = super.getElementById(id);

		if (!result) {
			for (var i = 0; i < this._columns.length; i++) {
				result = this._columns[i].getElementById(id);

				if (result) {
					break;
				}
			}
		}

		return result;
	}

	getActionById(id: string): Core.Action {
		var result: Core.Action = null;

		for (var i = 0; i < this._columns.length; i++) {
			result = this._columns[i].getActionById(id);

			if (result) {
				break;
			}
		}

		return result;
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak;
		}

		// render each item
		let speak = '';

		if (this._columns.length > 0) {
			for (var i = 0; i < this._columns.length; i++) {
				speak += this._columns[i].renderSpeech();
			}
		}

		return speak;
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