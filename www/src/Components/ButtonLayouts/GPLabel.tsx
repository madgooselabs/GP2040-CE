import React from 'react';
const GPLabel = ({x1, y1, x2, y2, stroke, fill, value, shape, angleStart, angleEnd, closed, onClick}) => {
	return (
		<text x={x1} y={y1} onClick={onClick}>{value}</text>
	);
};
export default GPLabel;