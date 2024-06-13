import React from 'react';
import { GPButton, GPLever, GPLabel, GPSprite, GPShape } from '.';

const GPController = ({className, layouts, children, itemSelect}) => {
	let buttonLayout = layouts?.displayLayouts.buttonLayout || [];
	let buttonLayoutRight = layouts?.displayLayouts.buttonLayoutRight || [];

	const createLayoutElement = (name, obj) => {
		if (!obj) return '';
		switch (obj.elementType) {
			case 2:
			case 3:
			case 4:
				return (
					<GPButton 
					key={`layout-base-button-${name}`}
					x1={obj.parameters.x1}
					y1={obj.parameters.y1}
					x2={obj.parameters.x2}
					y2={obj.parameters.y2}
					stroke={obj.parameters.stroke}
					fill={obj.parameters.fill}
					value={obj.parameters.value}
					shape={obj.parameters.shape}
					angleStart={obj.parameters.angleStart}
					angleEnd={obj.parameters.angleEnd}
					closed={obj.parameters.closed}
					clickEvent={itemSelect}
                    elementDetails={obj}
					/>
				);
			case 5:
				return (
					<GPLever 
					key={`layout-base-lever-${name}`}
					x1={obj.parameters.x1}
					y1={obj.parameters.y1}
					x2={obj.parameters.x2}
					y2={obj.parameters.y2}
					stroke={obj.parameters.stroke}
					fill={obj.parameters.fill}
					value={obj.parameters.value}
					shape={obj.parameters.shape}
					angleStart={obj.parameters.angleStart}
					angleEnd={obj.parameters.angleEnd}
					closed={obj.parameters.closed}
					clickEvent={itemSelect}
                    elementDetails={obj}
					/>
				);
			case 6:
				return (
					<GPLabel 
					key={`layout-base-label-${name}`}
					x1={obj.parameters.x1}
					y1={obj.parameters.y1}
					x2={obj.parameters.x2}
					y2={obj.parameters.y2}
					stroke={obj.parameters.stroke}
					fill={obj.parameters.fill}
					value={obj.parameters.value}
					shape={obj.parameters.shape}
					angleStart={obj.parameters.angleStart}
					angleEnd={obj.parameters.angleEnd}
					closed={obj.parameters.closed}
					clickEvent={itemSelect}
                    elementDetails={obj}
					/>
				);
			case 7:
				return (
					<GPSprite 
					key={`layout-base-sprite-${name}`} 
					x1={obj.parameters.x1}
					y1={obj.parameters.y1}
					x2={obj.parameters.x2}
					y2={obj.parameters.y2}
					stroke={obj.parameters.stroke}
					fill={obj.parameters.fill}
					value={obj.parameters.value}
					shape={obj.parameters.shape}
					angleStart={obj.parameters.angleStart}
					angleEnd={obj.parameters.angleEnd}
					closed={obj.parameters.closed}
					clickEvent={itemSelect}
                    elementDetails={obj}
					/>
				);
			case 8:
				return (
					<GPShape 
					key={`layout-base-shape-${name}`} 
					x1={obj.parameters.x1}
					y1={obj.parameters.y1}
					x2={obj.parameters.x2}
					y2={obj.parameters.y2}
					stroke={obj.parameters.stroke}
					fill={obj.parameters.fill}
					value={obj.parameters.value}
					shape={obj.parameters.shape}
					angleStart={obj.parameters.angleStart}
					angleEnd={obj.parameters.angleEnd}
					closed={obj.parameters.closed}
					clickEvent={itemSelect}
                    elementDetails={obj}
					/>
				);
		}
	};

	return (
		<div
		className={className}
		key={'button-layout-base'}
		>
			<div>
				<svg
				xmlns="http://www.w3.org/2000/svg"
				viewBox={`0 0 128 64`}
				>
					<defs>
						<filter id="invert-filter" colorInterpolationFilters="sRGB">
							<feComponentTransfer>
								<feFuncR type="table" tableValues="0.25 0"/>
								<feFuncG type="table" tableValues="0.25 0"/>
								<feFuncB type="table" tableValues="0.25 0"/>
							</feComponentTransfer>
						</filter>
					</defs>
					{buttonLayout ? Object.keys(buttonLayout).map((buttonID,id) => {
						return createLayoutElement(`left-${id}`, buttonLayout[buttonID]);
					}) : ''}
					{buttonLayoutRight ? Object.keys(buttonLayoutRight).map((buttonID,id) => {
						return createLayoutElement(`right-${id}`, buttonLayoutRight[buttonID]);
					}) : ''}
				</svg>
			</div>
			<div>
				{children}
			</div>
		</div>
	);
};

export default GPController;