import * as Core from "../card-elements";
import * as Utils from "../utils";
import * as HostConfig from "../host-config";

export class MediaSource {
	mimeType: string;
	url: string;

	constructor(url: string = undefined, mimeType: string = undefined) {
		this.url = url;
		this.mimeType = mimeType;
	}

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		this.mimeType = json["mimeType"];
		this.url = json["url"];
	}

	toJSON() {
		return {
			mimeType: this.mimeType,
			url: this.url
		}
	}
}

export class Media extends Core.CardElement {
	static readonly supportedMediaTypes = ["audio", "video"];

	private _selectedMediaType: string;
	private _selectedSources: Array<MediaSource>;

	private getPosterUrl(): string {
		return this.poster ? this.poster : this.hostConfig.media.defaultPoster;
	}

	private processSources() {
		this._selectedSources = [];
		this._selectedMediaType = undefined;

		for (let source of this.sources) {
			let mimeComponents = source.mimeType.split('/');

			if (mimeComponents.length == 2) {
				if (!this._selectedMediaType) {
					let index = Media.supportedMediaTypes.indexOf(mimeComponents[0]);

					if (index >= 0) {
						this._selectedMediaType = Media.supportedMediaTypes[index];
					}
				}
				if (mimeComponents[0] == this._selectedMediaType) {
					this._selectedSources.push(source);
				}
			}
		}
	}

	private renderPoster(): HTMLElement {
		const playButtonArrowWidth = 12;
		const playButtonArrowHeight = 15;

		let posterRootElement = document.createElement("div");
		posterRootElement.className = "ac-media-poster";
		posterRootElement.setAttribute("role", "contentinfo");
		posterRootElement.setAttribute("aria-label", this.altText ? this.altText : "Media content");
		posterRootElement.style.position = "relative";
		posterRootElement.style.display = "flex";

		let posterUrl = this.getPosterUrl();

		if (posterUrl) {
			let posterImageElement = document.createElement("img");
			posterImageElement.style.width = "100%";
			posterImageElement.style.height = "100%";

			posterImageElement.onerror = (e: Event) => {
				posterImageElement.parentNode.removeChild(posterImageElement);
				posterRootElement.classList.add("empty");
				posterRootElement.style.minHeight = "150px";
			}

			posterImageElement.src = posterUrl;

			posterRootElement.appendChild(posterImageElement);
		}
		else {
			posterRootElement.classList.add("empty");
			posterRootElement.style.minHeight = "150px";
		}

		if (this.hostConfig.supportsInteractivity && this._selectedSources.length > 0) {
			let playButtonOuterElement = document.createElement("div");
			playButtonOuterElement.setAttribute("role", "button");
			playButtonOuterElement.setAttribute("aria-label", "Play media");
			playButtonOuterElement.className = "ac-media-playButton";
			playButtonOuterElement.style.display = "flex";
			playButtonOuterElement.style.alignItems = "center";
			playButtonOuterElement.style.justifyContent = "center";
			playButtonOuterElement.onclick = (e) => {
				if (this.hostConfig.media.allowInlinePlayback) {
					let mediaPlayerElement = this.renderMediaPlayer();

					this.renderedElement.innerHTML = "";
					this.renderedElement.appendChild(mediaPlayerElement);

					mediaPlayerElement.play();
				}
				else {
					if (Media.onPlay) {
						Media.onPlay(this);
					}
				}
			}

			let playButtonInnerElement = document.createElement("div");
			playButtonInnerElement.className = "ac-media-playButton-arrow";
			playButtonInnerElement.style.width = playButtonArrowWidth + "px";
			playButtonInnerElement.style.height = playButtonArrowHeight + "px";
			playButtonInnerElement.style.borderTopWidth = (playButtonArrowHeight / 2) + "px";
			playButtonInnerElement.style.borderBottomWidth = (playButtonArrowHeight / 2) + "px";
			playButtonInnerElement.style.borderLeftWidth = playButtonArrowWidth + "px";
			playButtonInnerElement.style.borderRightWidth = "0";
			playButtonInnerElement.style.borderStyle = "solid";
			playButtonInnerElement.style.borderTopColor = "transparent";
			playButtonInnerElement.style.borderRightColor = "transparent";
			playButtonInnerElement.style.borderBottomColor = "transparent";
			playButtonInnerElement.style.transform = "translate(" + (playButtonArrowWidth / 10) + "px,0px)";

			playButtonOuterElement.appendChild(playButtonInnerElement);

			let playButtonContainer = document.createElement("div");
			playButtonContainer.style.position = "absolute";
			playButtonContainer.style.left = "0";
			playButtonContainer.style.top = "0";
			playButtonContainer.style.width = "100%";
			playButtonContainer.style.height = "100%";
			playButtonContainer.style.display = "flex";
			playButtonContainer.style.justifyContent = "center";
			playButtonContainer.style.alignItems = "center";

			playButtonContainer.appendChild(playButtonOuterElement);
			posterRootElement.appendChild(playButtonContainer);
		}

		return posterRootElement;
	}

	private renderMediaPlayer(): HTMLMediaElement {
		let mediaElement: HTMLMediaElement;

		if (this._selectedMediaType == "video") {
			let videoPlayer = document.createElement("video");

			let posterUrl = this.getPosterUrl();

			if (posterUrl) {
				videoPlayer.poster = posterUrl;
			}

			mediaElement = videoPlayer;
		}
		else {
			mediaElement = document.createElement("audio");
		}

		mediaElement.controls = true;
		mediaElement.preload = "none";
		mediaElement.style.width = "100%";

		for (let source of this.sources) {
			let src: HTMLSourceElement = document.createElement("source");
			src.src = source.url;
			src.type = source.mimeType;

			mediaElement.appendChild(src);
		}

		return mediaElement;
	}

	protected internalRender(): HTMLElement {
		let element = <HTMLElement>document.createElement("div");
		element.className = this.hostConfig.makeCssClassName("ac-media");

		this.processSources();

		element.appendChild(this.renderPoster());

		return element;
	}

	static onPlay: (sender: Media) => void;

	sources: Array<MediaSource> = [];
	poster: string;
	altText: string;

	parse(json: any, errors?: Array<HostConfig.IValidationError>) {
		super.parse(json, errors);

		this.poster = json["poster"];
		this.altText = json["altText"];

		if (json["sources"] != null) {
			let jsonSources = json["sources"] as Array<any>;

			this.sources = [];

			for (let i = 0; i < jsonSources.length; i++) {
				let source = new MediaSource();
				source.parse(jsonSources[i], errors);

				this.sources.push(source);
			}
		}
	}

	toJSON() {
		let result = super.toJSON();

		Utils.setProperty(result, "poster", this.poster);
		Utils.setProperty(result, "altText", this.altText);

		if (this.sources.length > 0) {
			let serializedSources = [];

			for (let source of this.sources) {
				serializedSources.push(source.toJSON());
			}

			Utils.setProperty(result, "sources", serializedSources);
		}

		return result;
	}

	getJsonTypeName(): string {
		return "Media";
	}

	getResourceInformation(): Array<Core.IResourceInformation> {
		let result: Array<Core.IResourceInformation> = [];

		let posterUrl = this.getPosterUrl();

		if (!Utils.isNullOrEmpty(posterUrl)) {
			result.push({ url: posterUrl, mimeType: "image" });
		}

		for (let mediaSource of this.sources) {
			if (!Utils.isNullOrEmpty(mediaSource.url)) {
				result.push({ url: mediaSource.url, mimeType: mediaSource.mimeType });
			}
		}

		return result;
	}

	renderSpeech(): string {
		return this.altText;
	}

	get selectedMediaType(): string {
		return this._selectedMediaType;
	}
}