import * as Core from "../card-elements";
import { Image } from "./image";
import * as Utils from "../utils";
import * as Enums from "../enums";
import * as HostConfig from "../host-config";

export class ImageSet extends Core.CardElementContainer {
	private _images: Array<Image> = [];

	protected internalRender(): HTMLElement {
		let element: HTMLElement = null;

		if (this._images.length > 0) {
			element = document.createElement("div");
			element.style.display = "flex";
			element.style.flexWrap = "wrap";

			for (var i = 0; i < this._images.length; i++) {
				this._images[i].size = this.imageSize;

				let renderedImage = this._images[i].render();

				renderedImage.style.display = "inline-flex";
				renderedImage.style.margin = "0px";
				renderedImage.style.marginRight = "10px";
				renderedImage.style.maxHeight = this.hostConfig.imageSet.maxImageHeight + "px";

				Utils.appendChild(element, renderedImage);
			}
		}

		return element;
	}

	imageSize: Enums.Size = Enums.Size.Medium;

	getItemCount(): number {
		return this._images.length;
	}

	getItemAt(index: number): Core.CardElement {
		return this._images[index];
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		let result: Array<Core.IResourceInformation> = [];

		for (let image of this._images) {
			result = result.concat(image.getResourceInformation());
		}

		return result;
	}

	removeItem(item: Core.CardElement): boolean {
		if (item instanceof Image) {
			var itemIndex = this._images.indexOf(item);

			if (itemIndex >= 0) {
				this._images.splice(itemIndex, 1);

				item.setParent(null);

				this.updateLayout();

				return true;
			}
		}

		return false;
	}

	getJsonTypeName(): string {
		return "ImageSet";
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setEnumProperty(Enums.Size, result, "imageSize", this.imageSize, Enums.Size.Medium);

		if (this._images.length > 0) {
			let images = [];

			for (let image of this._images) {
				images.push(image.toJSON());
			}

			Utils.setProperty(result, "images", images);
		}

		return result;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.imageSize = Utils.getEnumValueOrDefault(Enums.Size, json["imageSize"], Enums.Size.Medium);

		if (json["images"] != null) {
			let jsonImages = json["images"] as Array<any>;

			this._images = [];

			for (let i = 0; i < jsonImages.length; i++) {
				var image = new Image();
				image.parse(jsonImages[i], errors);

				this.addImage(image);
			}
		}
	}

	addImage(image: Image) {
		if (!image.parent) {
			this._images.push(image);

			image.setParent(this);
		}
		else {
			throw new Error("This image already belongs to another ImageSet");
		}
	}

	indexOf(cardElement: Core.CardElement): number {
		return cardElement instanceof Image ? this._images.indexOf(cardElement) : -1;
	}

	renderSpeech(): string {
		if (this.speak != null) {
			return this.speak;
		}

		var speak = null;

		if (this._images.length > 0) {
			speak = '';

			for (var i = 0; i < this._images.length; i++) {
				speak += this._images[i].renderSpeech();
			}
		}

		return speak;
	}
}