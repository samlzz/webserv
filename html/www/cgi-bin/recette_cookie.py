#!/usr/bin/env python3
import os
import sys

#contenu HTML
html_content = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Recette de Cookies</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
        }
        .container {
            background: white;
            border-radius: 15px;
            padding: 40px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        h1 {
            color: #764ba2;
            text-align: center;
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        h2 {
            color: #667eea;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
            margin-top: 30px;
        }
        .step {
            background: #f8f9fa;
            border-left: 4px solid #764ba2;
            padding: 15px;
            margin: 15px 0;
            border-radius: 5px;
            transition: transform 0.2s;
        }
        .step:hover {
            transform: translateX(5px);
            box-shadow: 0 3px 10px rgba(0,0,0,0.1);
        }
        .step-number {
            font-weight: bold;
            color: #764ba2;
            font-size: 1.2em;
        }
        .footer {
            text-align: center;
            margin-top: 30px;
            color: #666;
            font-style: italic;
        }
        .emoji {
            font-size: 1.5em;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1><span class="emoji">🍪</span> Recette de Cookies <span class="emoji">🍪</span></h1>
        
        <h2>Ingrédients</h2>
        <p style="text-align: center; color: #764ba2; font-weight: bold; font-size: 1.1em;">Pour faire 30 cookies</p>
        
        <div style="background: #f0f4ff; padding: 20px; border-radius: 10px; margin: 20px 0;">
            <ul style="list-style: none; padding: 0;">
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>250 g</strong> de beurre ou de margarine</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>1</strong> œuf</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>170 g</strong> de cassonade</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>170 g</strong> de vergeoise blonde</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>400 g</strong> de farine de blé type T55</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>425 g</strong> de pépites de chocolat</li>
                <li style="padding: 8px 0; border-bottom: 1px solid #e0e0e0;"><strong>1 sachet</strong> de levure sans phosphate</li>
                <li style="padding: 8px 0;"><strong>2 pincées</strong> de sel</li>
            </ul>
        </div>
        
        <h2>Préparation</h2>
        
        <div class="step">
            <span class="step-number">1. Prêt, partez !</span><br>
            Pesez tous les ingrédients scrupuleusement et préparez votre matériel.
        </div>
        
        <div class="step">
            <span class="step-number">2. Le beurre</span><br>
            Dans un grand récipient tout rond, déposez le beurre ramolli (il doit avoir la consistance d'une pommade).
        </div>
        
        <div class="step">
            <span class="step-number">3. Le sucre</span><br>
            Versez ensuite le sucre dessus et mélangez le tout à l'aide d'une spatule.
        </div>
        
        <div class="step">
            <span class="step-number">4. L'œuf</span><br>
            Ajoutez l'œuf et mélangez de nouveau l'ensemble (l'œuf doit être complètement incorporé à la préparation).
        </div>
        
        <div class="step">
            <span class="step-number">5. Les poudres</span><br>
            À part, tamisez les poudres : la farine, la levure.
        </div>
        
        <div class="step">
            <span class="step-number">6. Mélange</span><br>
            Incorporez les poudres au premier mélange (beurre, sucre, œuf).
        </div>
        
        <div class="step">
            <span class="step-number">7. Les extras</span><br>
            Ajoutez la fleur de sel, les pépites de chocolat (et éventuels autres ingrédients pour les plus gourmands) et poursuivre le mélange, jusqu'à ce que le tout soit joliment homogène.
        </div>
        
        <div class="step">
            <span class="step-number">8. Former les cookies</span><br>
            Munissez-vous d'une cuillère à glace, remplissez-la de pâte, puis formez le cookie. Le cookie doit peser 50g à peu près. Aplatissez légèrement chaque cookie et conservez au frais pendant 2h minimum.
        </div>
        
        <h2>Cuisson</h2>
        
        <div class="step">
            <span class="step-number">9. Chaud devant !</span><br>
            Préchauffez votre four à température moyenne (140° ou thermostat 4-5).
        </div>
        
        <div class="step">
            <span class="step-number">10. Disposition</span><br>
            Disposez vos boules de pâte sur une plaque de cuisson anti-adhésive ou recouverte d'une feuille de papier cuisson.
        </div>
        
        <div class="step">
            <span class="step-number">11. Au four</span><br>
            Il ne vous reste plus qu'à enfourner vos cookies pendant 15 minutes.
        </div>
        
        <div class="step">
            <span class="step-number">12. Dégustation</span><br>
            Après cuisson, l'idéal est d'attendre une quinzaine de minutes avant dégustation, vos cookies seront alors tièdes et parfaitement délicieux. Il ne vous reste plus qu'à déguster vos cookies maison.
        </div>
        
        <div class="footer">
            <span class="emoji">✨</span> Bon appétit ! <span class="emoji">✨</span>
        </div>
    </div>
</body>
</html>
"""


content_length = len(html_content.encode('utf-8'))

print("Content-Type: text/html; charset=utf-8")
print(f"Content-Length: {content_length}")
print() #\r\n

print(html_content, end='')
