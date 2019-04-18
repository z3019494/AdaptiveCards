import { HostContainer } from "./host-container";
import {
    AdaptiveCard,
    HostConfig,
    Size,
    TextSize,
    TextColor,
    TextWeight,
    Spacing,
    ShowCardActionMode,
    Orientation,
    ActionAlignment,
} from "adaptivecards";
import * as webChatConfiguration from "../../../../../samples/HostConfig/webchat.json";

export class WebChatContainer extends HostContainer {
    protected renderContainer(adaptiveCard: AdaptiveCard, target: HTMLElement): HTMLElement {
        let outerElement = document.createElement("div");
        outerElement.className = "webChatOuterContainer";

        let resizeCard = () => {
            if (outerElement.parentElement) {
                let bounds = outerElement.parentElement.getBoundingClientRect();
                let newWidth: string = "216px";

                if (bounds.width >= 500) {
                    newWidth = "416px";
                }
                else if (bounds.width >= 400) {
                    newWidth = "320px";
                }

                if (outerElement.style.width !== newWidth) {
                    outerElement.style.width = newWidth;
                }

                adaptiveCard.updateLayout();
            }
        };

        window.addEventListener("resize", resizeCard);

        let innerElement = document.createElement("div");
        innerElement.className = "webChatInnerContainer";

        target.appendChild(outerElement);
        outerElement.appendChild(innerElement);

        let renderedCard = adaptiveCard.render();
        innerElement.appendChild(renderedCard);
        resizeCard();

        return outerElement;
    }

    public getHostConfig(): HostConfig {
        return new HostConfig(webChatConfiguration);
    }
}
