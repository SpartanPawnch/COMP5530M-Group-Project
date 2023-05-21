local M {
	update=function(dt,entityState)
		local speedX=-10.0
		local speedAngular=30.0
		entityState.position:add3f(speedX*dt,.0,.0);
		entityState.rotation:rotateAngleAxis(speedAngular,.0,1.0,.0);
	end
}
return M面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面面