import React from 'react';
const GPSprite = ({x1, y1, x2, y2, stroke, fill, value, shape, angleStart, angleEnd, closed, onClick}) => {
	return (
		<image x={x1} y={y1} width={x2} height={y2} href={`#${value}`} />
	);
};
export default GPSprite;