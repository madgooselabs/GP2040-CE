import React from 'react';
import GPShape from './GPShape';

const GPLever = ({x1, y1, x2, y2, stroke, fill, value, shape, angleStart, angleEnd, closed, onClick}) => {
    return (
        <g>
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
            <GPShape
                x1={x1}
                y1={y1}
                x2={x2*0.75}
                y2={y2*0.75}
                stroke={stroke}
                fill={1}
                value={value}
                shape={shape}
                angleStart={angleStart}
                angleEnd={angleEnd}
                closed={closed}
                onClick={onClick}
            />
        </g>
    );
};

export default GPLever;