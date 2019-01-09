import { TextBlock } from "./text-block";
import * as Utils from "../utils";

export class Label extends TextBlock {
	protected getRenderedDomElementType(): string {
		return "label";
	}

	protected internalRender(): HTMLElement {
		let renderedElement = <HTMLLabelElement>super.internalRender();

		if (!Utils.isNullOrEmpty(this.forElementId)) {
			renderedElement.htmlFor = this.forElementId;
		}

		return renderedElement;
	}

	forElementId: string;
}