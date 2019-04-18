import * as Adaptive from "adaptivecards";
import * as Designer from "../../adaptivecards-designer";

var skypeConfiguration = require("../../../../../../samples/HostConfig/skype.json");

export class SkypeContainer extends Designer.HostContainer {
    public renderTo(hostElement: HTMLElement) {
        this.cardHost.classList.add("skype-card");

        let frame = document.createElement("div");
        frame.className = "skype-frame";

        // Draw the hexagon bot logo
        let hexagonOuter = document.createElement("div");
        hexagonOuter.className = "skype-hexagon-outer";

        let hexagonInner = document.createElement("div");
        hexagonInner.className = "skype-hexagon-inner";

        let botLogo = document.createElement("div");
        botLogo.className = "skype-bot-logo";

        hexagonOuter.appendChild(hexagonInner);
        hexagonInner.appendChild(botLogo);

        frame.appendChild(hexagonOuter);
        frame.appendChild(this.cardHost);

        hostElement.appendChild(frame);
    }

    public getHostConfig(): Adaptive.HostConfig {
        return new Adaptive.HostConfig(skypeConfiguration);
    }
}
