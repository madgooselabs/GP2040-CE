import { create } from 'zustand';
import WebApi from '../Services/WebApi';

type State = {
	buttonLayoutsLeft: {
        label: string,
        value: number,
    }[];
	buttonLayoutsRight: {
        label: string,
        value: number,
    }[];
	layoutSettings: {
		ledLayout:{},
		displayLayouts:{
			buttonLayoutId:0,
			buttonLayout:{},
			buttonLayoutRightId:0,
			buttonLayoutRight:{}
		}
	};
};

type Actions = {
	fetchLayoutDetails: () => void;
	fetchLayoutDefinitions: () => void;
};

const INITIAL_STATE: State = {
	buttonLayoutsLeft:[],
	buttonLayoutsRight:[],
	layoutSettings: {
		ledLayout:{},
		displayLayouts:{
			buttonLayoutId:0,
			buttonLayout:{},
			buttonLayoutRightId:0,
			buttonLayoutRight:{}
		}
	},
};

const useLayoutStore = create<State & Actions>()((set, get) => ({
	...INITIAL_STATE,
	fetchLayoutDefinitions: async () => {
		const defs = await WebApi.getButtonLayoutDefs();
		set((state) => ({
			...state,
			buttonLayoutsLeft: Object.entries(defs.buttonLayout).map(([key, val]) => ({label: key, value: val})),
			buttonLayoutsRight: Object.entries(defs.buttonLayoutRight).map(([key, val]) => ({label: key, value: val})),
		}));
	},
	fetchLayoutDetails: async () => {
		const layouts = await WebApi.getButtonLayouts();
		set((state) => ({
			...state,
			layoutSettings: layouts,
		}));
	},
}));

export default useLayoutStore;
