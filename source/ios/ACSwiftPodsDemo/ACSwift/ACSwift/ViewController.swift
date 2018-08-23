//
//  ViewController.swift
//  ACSwift
//
//  Created by Inyoung Woo on 8/23/18.
//  Copyright © 2018 Inyoung Woo. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        let path : String = Bundle.main.path(forResource:"microsoft-teams", ofType:"json")!;
        let hostConfig : String = try! String(contentsOfFile: path, encoding: String.Encoding.utf8);
        
        let parseResult = ACOHostConfig.fromJson(hostConfig);
        if(parseResult?.isValid)! {
            let path : String = Bundle.main.path(forResource:"WeatherLarge", ofType:"json")!;
            let payload : String = try! String(contentsOfFile: path, encoding: String.Encoding.utf8);
            let card : ACOAdaptiveCardParseResult = ACOAdaptiveCard.fromJson(payload);
            let renderResult = ACRRenderer.render(card.card, config: parseResult?.config, widthConstraint: 300);
            self.view.addSubview((renderResult?.view)!);
            NSLayoutConstraint.activate([(renderResult?.view.topAnchor.constraint(equalTo: self.view.topAnchor, constant:50))!, (renderResult?.view.centerXAnchor.constraint(equalTo: self.view.centerXAnchor, constant:0))!]);
	        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

