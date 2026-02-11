/**
 * Welcome to Cloudflare Workers! This is your first worker.
 *
 * - Run `npm run dev` in your terminal to start a development server
 * - Open a browser tab at http://localhost:8787/ to see your worker in action
 * - Run `npm run deploy` to publish your worker
 *
 * Bind resources to your worker in `wrangler.jsonc`. After adding bindings, a type definition for the
 * `Env` object can be regenerated with `npm run cf-typegen`.
 *
 * Learn more at https://developers.cloudflare.com/workers/
 */

export default {
	async fetch(request, env, ctx) {
		try {
			const url = new URL(request.url);

			// Only handle the /departures path
			if (url.pathname !== "/departures") {
				return new Response("Not Found", { status: 404 });
			}

			// Parse query parameters
			const countParam = url.searchParams.get("count");
			const toTownParam = url.searchParams.get("toTown");

			const count = countParam ? parseInt(countParam) : 5;
			const toTown = toTownParam === "true";

			// Simple validation
			if (isNaN(count) || count < 1) {
				return new Response(
					JSON.stringify({ error: "Invalid 'count' parameter" }),
					{ status: 400, headers: { "Content-Type": "application/json" } }
				);
			}

			// Mock departures data
			const allDepartures = [
				"08:00 Bus A",
				"08:15 Bus B",
				"08:30 Bus C",
				"09:00 Bus D",
				"09:15 Bus E",
				"09:30 Bus F",
				"10:00 Bus G",
				"10:15 Bus H",
			];

			// Filter based on toTown (just an example filter)
			const filtered = allDepartures.filter((dep, i) =>
				toTown ? i % 2 === 0 : i % 2 !== 0
			);

			// Limit to requested count
			const departures = filtered.slice(0, count);

					// Get client IP from Cloudflare header
			const clientIP = request.headers.get("CF-Connecting-IP") || "8.8.8.8"; // fallback

			// Fetch timezone offset (in seconds) from ip-api.com
			let timeZoneOffset = 0; // default to UTC
			try {
				const resp = await fetch(`http://ip-api.com/json/${clientIP}?fields=timezone`);
				const data = await resp.json();
				if (data.timezone) {
					// Compute offset in milliseconds
					const localDate = new Date().toLocaleString("en-US", { timeZone: data.timezone });
					timeZoneOffset = new Date(localDate).getTime() - Date.now();
				}
			} catch (err) {
				// If IP lookup fails, fallback to UTC
				timeZoneOffset = 0;
			}

			const result = {
				time: Date.now() + timeZoneOffset,
				departures,
			};

			return new Response(JSON.stringify(result), {
				status: 200,
				headers: { "Content-Type": "application/json" },
			});
		} catch (err) {
			return new Response(
				JSON.stringify({ error: "Internal Server Error" }),
				{ status: 500, headers: { "Content-Type": "application/json" } }
			);
		}
	},
} satisfies ExportedHandler<Env>;