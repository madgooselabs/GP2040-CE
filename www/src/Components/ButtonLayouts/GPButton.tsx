import React from 'react';
import GPShape from './GPShape';
const GPButton = ({x1, y1, x2, y2, stroke, fill, value, shape, angleStart, angleEnd, closed, onClick}) => {
	return (
		<GPShape
			x1={x1}
			y1={y1}
			x2={x2}
			y2={y2}
			stroke={stroke}
			fill={fill}
			value={value}
			shape={shape}
			angleStart={angleStart}
			angleEnd={angleEnd}
			closed={closed}
			onClick={onClick}
		/>
	);
};
export default GPButton;