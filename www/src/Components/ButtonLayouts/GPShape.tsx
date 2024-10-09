import React from 'react';
const GPShape = ({x1, y1, x2, y2, stroke, fill, value, shape, angleStart, angleEnd, closed, onClick}) => {
	let strokeValue = "none";
	let fillValue = "none";
	let filter = "";
	switch (stroke) {
		case 0:
			strokeValue = "none";
			break;
		case 1:
			strokeValue = "black";
			break;
		case 2:
			strokeValue = "black";
			filter = "url(#invert-filter)";
			break;
	}
	switch (fill) {
		case 0:
			fillValue = "none";
			break;
		case 1:
			fillValue = "black";
			break;
		case 2:
			fillValue = "black";
			filter = "url(#invert-filter)";
			break;
	}
	switch (shape) {
		case 0:
			// ellipse
			return (
				<ellipse cx={x1} cy={y1} rx={x2} ry={y2} fill={fillValue} filter={filter} stroke={strokeValue} onClick={onClick} />
			);
		case 1:
			// square
			return (
				<rect x={x1} y={y1} width={x2-x1} height={y2-y1} filter={filter} fill={fillValue} stroke={strokeValue} onClick={onClick} />
			);
		case 2:
			// diamond
			return (
				<rect x={x1-x2} y={y1-y2} width={x2*2} height={y2*2} transform={`rotate(45 ${x1+x2} ${y1+y2})`} filter={filter} fill={fillValue} stroke={strokeValue} onClick={onClick} />
			);
		case 3:
			// polygon
			let points = "";
			let angleIncrement = 2 * Math.PI / y2;
			for (let i = 0; i < y2; i++) {
				let angle = i * angleIncrement + angleStart;
				let x = x1 + Math.round(x2 * Math.cos(angle));
				let y = y1 + Math.round(x2 * Math.sin(angle));
				points += x + "," + y + " ";
			}
			
			return (
				<polygon points={points.trim()}  filter={filter} fill={fillValue} stroke={strokeValue} onClick={onClick} />
			);
		case 4:
			// arc
			const startAngle = (angleStart * Math.PI) / 180;
			const endAngle = (angleEnd * Math.PI) / 180;
			const startX = x1 + x2 * Math.cos(startAngle);
			const startY = y1 + y2 * Math.sin(startAngle);
			const endX = x1 + x2 * Math.cos(endAngle);
			const endY = y1 + y2 * Math.sin(endAngle);
			return (
				<path d={`M${startX},${startY} A${x2},${y2} 0 0 1 ${endX},${endY} L${x1},${y1} Z`} filter={filter} fill={fillValue} stroke={strokeValue} onClick={onClick} />
			);
		default:
			// nothing
	}
};
export default GPShape;